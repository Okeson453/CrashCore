#pragma once
/**
 * Full CrashCore event engine — composes all PORT_* subsystems into one
 * runnable processing unit matching TestingEngine live event flow:
 *
 *   WS frame
 *     → Engine.IO / Socket.IO parse
 *     → CrashEvent decode
 *     → Realtime normalize + validate
 *     → Duplicate filter
 *     → Round state machine
 *     → Route (prediction | validation | persistence queues)
 *     → Prediction attempt coordinator (external PE)
 *     → Live validator on ED
 *     → Feedback + loss cooldown
 *     → Outbox lifecycle + Telegram dispatcher
 *     → Optional persistence batch
 *
 * Prediction models / ACIE / features are EXCLUDE — never called here.
 */
#include "application/pipeline_loop.hpp"
#include "ingestion/event_decoder.hpp"
#include "ingestion/event_router.hpp"
#include "ingestion/native_bc_socket.hpp"
#include "ingestion/signing/native_sign.hpp"
#include "ingestion/socket_diagnostics.hpp"
#include "ingestion/stats.hpp"
#include "ingestion/socketio/protocol_edge_cases.hpp"
#include "realtime/realtime_pipeline.hpp"
#include "prediction_interface/prediction_client.hpp"
#include "prediction_interface/prediction_attempt.hpp"
#include "validation/live_validator.hpp"
#include "validation/loss_cooldown.hpp"
#include "validation/stuck_recovery.hpp"
#include "validation/invariants.hpp"
#include "validation/fencing.hpp"
#include "delivery/outbox/outbox.hpp"
#include "delivery/outbox/outbox_lifecycle.hpp"
#include "delivery/telegram/telegram_dispatcher.hpp"
#include "delivery/signal_formatter.hpp"
#include "persistence/database.hpp"
#include "persistence/batch_writer.hpp"
#include "persistence/repositories/crash_round_repository.hpp"
#include "persistence/repositories/prediction_repository.hpp"
#include "persistence/repositories/outcome_repository.hpp"
#include "security/secret_provider.hpp"
#include "timing/latency_tracker.hpp"
#include "timing/clock_offset.hpp"
#include "telemetry/metrics.hpp"
#include "logging/structured_logger.hpp"
#include "common/result.hpp"
#include <memory>
#include <atomic>
#include <string>

namespace crashcore {

struct EventEngineConfig {
  std::size_t queueCapacity = 4096;
  std::size_t outboxCapacity = 2048;
  bool enableTelegram = false;
  bool enablePersistence = false;
  std::int64_t predictionTimeoutMs = 50;
  std::int64_t stuckMaxAgeMs = 180000;
};

class EventEngine {
public:
  explicit EventEngine(SecretProvider& secrets, EventEngineConfig cfg = {})
      : secrets_(secrets)
      , cfg_(cfg)
      , latency_()
      , metrics_()
      , decoder_(&latency_)
      , router_(cfg.queueCapacity, &latency_)
      , sign_(secrets)
      , outbox_(cfg.outboxCapacity)
      , outbox_life_(outbox_)
      , pred_client_(PredictionContract{}, &latency_)
      , live_validator_(&latency_)
      , pipeline_(decoder_, router_, pred_client_, live_validator_, outbox_life_, &latency_)
      , stuck_(live_validator_.registry(), cfg.stuckMaxAgeMs)
      , log_(ComponentId::Application) {
    pred_client_.setResponseHandler([this](const PredictionResponse& r) {
      pipeline_.onPredictionResponse(r);
    });
    live_validator_.setOutcomeHandler([this](const Outcome& o) {
      cooldown_.noteOutcome(o);
      invariants_.checkOutcome(o);
      pipeline_.onOutcome(o);
      if (outcome_repo_) outcome_repo_->upsert(o);
    });
  }

  Result<void> start() {
    if (cfg_.enableTelegram) {
      auto tok = secrets_.get("TELEGRAM_BOT_TOKEN");
      auto chat = secrets_.get("TELEGRAM_CHAT_ID");
      if (tok && chat) {
        TelegramCredentials c;
        c.botToken = *tok;
        c.chatId = *chat;
        telegram_ = std::make_unique<TelegramDispatcher>(c);
      }
    }
    if (cfg_.enablePersistence) {
      DatabaseCredentials dc;
      if (auto u = secrets_.get("DATABASE_URL")) dc.connectionString = *u;
      db_ = std::make_unique<Database>(dc);
      db_->connect();
      round_repo_ = std::make_unique<CrashRoundRepository>(*db_);
      pred_repo_ = std::make_unique<PredictionRepository>(*db_);
      outcome_repo_ = std::make_unique<OutcomeRepository>(*db_);
      batch_ = std::make_unique<BatchWriter>(db_.get());
      batch_->start();
    }
    native_ = std::make_unique<NativeBcGameSocket>(decoder_, router_, &sign_, &latency_);
    native_->onEvent([this](const CrashEvent& ev) {
      diagnostics_.note("event", ev.rawEventName, native_->framesRx(), native_->health().reconnects);
      invariants_.checkEndEvent(ev);
      if (ev.kind == EventKind::End && round_repo_) {
        round_repo_->upsertFromEvent(ev);
      }
    });
    // inject path works without live network
    auto r = native_->start();
    if (!r) log_.warn("native start: ", r.error().message);
    running_.store(true);
    log_.info("EventEngine started");
    return Result<void>::success();
  }

  void stop() {
    running_.store(false);
    if (native_) native_->stop();
    if (batch_) batch_->stop();
    log_.info("EventEngine stopped");
  }

  Result<void> processFrame(const std::uint8_t* data, std::size_t len) {
    if (!running_.load()) return Error{ErrorCode::Shutdown, "engine stopped"};
    diagnostics_.noteFrame(len);
    auto cls = classifyFrame(data, len);
    if (cls == FrameClass::Malformed || cls == FrameClass::Truncated) {
      diagnostics_.noteError("malformed frame");
      return Error{ErrorCode::ParseError, "malformed"};
    }
    // Prefer pipeline for full path
    auto r = pipeline_.onFrame(data, len);
    // Also inject into native socket packet path for state tracking
    if (native_) native_->injectFrame(data, len);
    return r;
  }

  /** Process a synthetic full round (tests). */
  Result<void> processSyntheticRound(std::uint64_t gameId, double finalMult) {
    auto frames = syntheticRoundFrames(gameId, finalMult, 8);
    for (const auto& f : frames) {
      auto r = processFrame(f.data(), f.size());
      if (!r && r.error().code != ErrorCode::ParseError) return r;
    }
    pipeline_.drain();
    stuck_.scan();
    return Result<void>::success();
  }

  PipelineLoop& pipeline() noexcept { return pipeline_; }
  LiveValidator& validator() noexcept { return live_validator_; }
  OutboxLifecycle& outbox() noexcept { return outbox_life_; }
  SocketDiagnostics& diagnostics() noexcept { return diagnostics_; }
  Metrics& metrics() noexcept { return metrics_; }
  LossCooldownTracker& cooldown() noexcept { return cooldown_; }
  InvariantChecker& invariants() noexcept { return invariants_; }
  bool running() const noexcept { return running_.load(); }

private:
  SecretProvider& secrets_;
  EventEngineConfig cfg_;
  LatencyTracker latency_;
  Metrics metrics_;
  EventDecoder decoder_;
  EventRouter router_;
  NativeSign sign_;
  Outbox outbox_;
  OutboxLifecycle outbox_life_;
  PredictionClient pred_client_;
  LiveValidator live_validator_;
  PipelineLoop pipeline_;
  StuckRecovery stuck_;
  LossCooldownTracker cooldown_;
  InvariantChecker invariants_;
  SocketDiagnostics diagnostics_;
  std::unique_ptr<NativeBcGameSocket> native_;
  std::unique_ptr<TelegramDispatcher> telegram_;
  std::unique_ptr<Database> db_;
  std::unique_ptr<CrashRoundRepository> round_repo_;
  std::unique_ptr<PredictionRepository> pred_repo_;
  std::unique_ptr<OutcomeRepository> outcome_repo_;
  std::unique_ptr<BatchWriter> batch_;
  StructuredLogger log_;
  std::atomic<bool> running_{false};
};

} // namespace crashcore
