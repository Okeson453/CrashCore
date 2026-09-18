#pragma once
/**
 * Top-level CrashCore application.
 * Wires: NativeBcGameSocket → EventEngine/PipelineLoop → prediction interface
 * → LiveValidator → Outbox → Telegram, with workers off the hot path.
 */
#include "application/application_config.hpp"
#include "application/lifecycle.hpp"
#include "application/event_engine.hpp"
#include "application/pipeline_loop.hpp"
#include "ingestion/event_decoder.hpp"
#include "ingestion/event_router.hpp"
#include "ingestion/signing/native_sign.hpp"
#include "ingestion/native_bc_socket.hpp"
#include "ingestion/round_state.hpp"
#include "ingestion/socket_diagnostics.hpp"
#include "prediction_interface/prediction_client.hpp"
#include "prediction_interface/in_process_prediction_client.hpp"
#include "prediction_interface/n1_coordinator.hpp"
#include "ingestion/rest/poll_worker.hpp"
#include "ingestion/rest/crash_history_client.hpp"
#include "delivery/http/curl_http_client.hpp"
#include "prediction_interface/prediction_response.hpp"
#include "prediction_interface/prediction_attempt.hpp"
#include "prediction_interface/rolling_history.hpp"
#include "prediction_interface/target_coordinator.hpp"
#include "validation/live_validator.hpp"
#include "validation/loss_cooldown.hpp"
#include "validation/fencing.hpp"
#include "validation/invariants.hpp"
#include "delivery/outbox/outbox.hpp"
#include "delivery/outbox/outbox_worker.hpp"
#include "delivery/outbox/outbox_lifecycle.hpp"
#include "delivery/outbox/durable_handoff.hpp"
#include "delivery/outbox/durable_prediction_tx.hpp"
#include "delivery/outbox/temporal_auth.hpp"
#include "prediction/acie/adaptive_edge.hpp"
#include "prediction/acie/strategy.hpp"
#include "feedback/exactly_once_feedback.hpp"
#include "feedback/delivery_forensics.hpp"
#include "observability/funnel_metrics.hpp"
#include "delivery/signal.hpp"
#include "delivery/signal_formatter.hpp"
#include "delivery/telegram/telegram_client.hpp"
#include "delivery/telegram/telegram_dispatcher.hpp"
#include "realtime/realtime_pipeline.hpp"
#include "concurrency/worker_pool.hpp"
#include "security/secret_provider.hpp"
#include "security/secure_config.hpp"
#include "timing/latency_tracker.hpp"
#include "timing/latency_budgets.hpp"
#include "timing/clock.hpp"
#include "timing/clock_offset.hpp"
#include "telemetry/metrics.hpp"
#include "telemetry/event_loop_lag.hpp"
#include "telemetry/metric_registry.hpp"
#include "telemetry/health_aggregator.hpp"
#include "orchestration/component_registry.hpp"
#include "orchestration/orchestrator.hpp"
#include "persistence/database.hpp"
#include "persistence/pg/pg_database.hpp"
#include "api/statistics_server.hpp"
#include "delivery/notification_worker.hpp"
#include "live/live_boot.hpp"
#include "live/live_supervisor.hpp"
#include "live/cold_start_seeder.hpp"
#include "live/clock_skew_monitor.hpp"
#include "persistence/pg/pg_database.hpp"
#include "persistence/batch_writer.hpp"
#include "persistence/migration_runner.hpp"
#include "persistence/retention.hpp"
#include "persistence/outbox_repository.hpp"
#include "config/toml_config.hpp"
#include "logging/logger.hpp"
#include "logging/structured_logger.hpp"
#include "common/result.hpp"
#include <memory>
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <chrono>
#include <sstream>

namespace crashcore {

class Application {
public:
  explicit Application(ApplicationConfig config = {})
      : config_(std::move(config))
      , secrets_(std::make_unique<EnvSecretProvider>())
      , latency_()
      , metrics_()
      , metric_reg_()
      , decoder_(&latency_)
      , router_(config_.eventQueueCapacity, &latency_)
      , sign_(*secrets_)
      , outbox_(config_.outboxCapacity)
      , outbox_life_(outbox_)
      , handoff_(outbox_)
      , prediction_(InProcessPredictionClient::Options{}, PredictionContract{}, &latency_)
      , fence_()
      , attempts_(prediction_, fence_, &latency_, config_.predictionTimeoutMs)
      , live_validator_(&latency_)
      , workers_(config_.workerCount)
      , registry_()
      , history_(200)
      , targets_()
      , cooldown_(config_.lossCooldownThreshold, config_.lossCooldownMs)
      , invariants_()
      , clock_offset_()
      , diagnostics_()
      , log_(ComponentId::Application) {}

  Result<void> loadConfigFile(const std::string& path) {
    TomlConfig toml;
    auto r = toml.loadFile(path);
    if (!r) return r.error();
    config_ = toml.toApplicationConfig();
    return Result<void>::success();
  }

  Result<void> init() {
    Lifecycle::installSignalHandlers();
    auto cfg = loadSecureConfig(*secrets_);
    if (!cfg) return cfg.error();
    secure_ = cfg.value();

    applyLogLevel(config_.logLevel);
    // Apply strategy / temporal config from ApplicationConfig
    {
      InProcessPredictionClient::Options o = prediction_.options();
      o.minConfidence = config_.minConfidence;
      o.defaultTarget = 1.30;
      o.qualityEdge = config_.qualityEdge;
      o.reducedEdge = config_.reducedEdge;
      prediction_.setOptions(o);
      temporal_.config().deliveryDeadlineMs = config_.deliveryDeadlineMs;
      adaptive_edge_.config().initialEdge = config_.qualityEdge;
    }

    registry_.registerComponent(ComponentId::Application, "application", 0);
    registry_.registerComponent(ComponentId::Ingestion, "ingestion", 1);
    registry_.registerComponent(ComponentId::PredictionInterface, "prediction", 2);
    registry_.registerComponent(ComponentId::Validation, "validation", 3);
    registry_.registerComponent(ComponentId::Outbox, "outbox", 4);

    // Native socket + engine
    native_socket_ = std::make_unique<NativeBcGameSocket>(
        decoder_, router_, &sign_, &latency_);
    native_socket_->onEvent([this](const CrashEvent& ev) {
      metrics_.inc(Metrics::EventsReceived);
      metric_reg_.counterInc(METRIC_EVENTS_RECEIVED);
      history_.pushFromEvent(ev);
      if (ev.kind == EventKind::Start || ev.kind == EventKind::Prepare) {
        const auto rid = ev.gameId.empty() ? ev.roundId : ev.gameId;
        targets_.noteRoundStart(rid);
        // Phase 1: BG temporal stamp + stale outbox kill
        temporal_.noteTargetStarted(rid, ev.beganAtMs > 0 ? ev.beganAtMs : nowMs());
        if (db_) {
          auto n = temporal_.killStaleForTarget(db_.get(), rid);
          if (n) log_.info("stale-kill target=", rid);
        }
      }
      invariants_.checkEndEvent(ev);
      diagnostics_.noteFrame(0);
      if (config_.enableClockSkewMonitor) {
        if (ev.endedAtMs > 0) skew_monitor_.observe(ev.endedAtMs);
        else if (ev.beganAtMs > 0) skew_monitor_.observe(ev.beganAtMs);
        if (ev.endedAtMs > 0) clock_offset_.observeServerTime(ev.endedAtMs);
      }
      funnel_.noteFrame();
      if (ev.valid) funnel_.noteDecoded();
    });
    native_socket_->onStatus([this](NativeSocketStatus st, std::string_view detail) {
      diagnostics_.noteStatus(st, std::string(detail));
      if (st == NativeSocketStatus::Connected)
        registry_.setState(ComponentId::Ingestion, ComponentState::Running);
    });

    // Pipeline loop composes decode→route→attempt→validate→outbox
    // Application owns response/outcome handlers — disable PipelineLoop handlers
    pipeline_ = std::make_unique<PipelineLoop>(
        decoder_, router_, prediction_, live_validator_, outbox_life_, &latency_,
        /*installHandlers=*/false);

    // Prediction responses → registry + outbox (durable handoff order)
    
    n1_ = std::make_unique<N1Coordinator>(prediction_, live_validator_, &handoff_, &metrics_, &latency_);
    // Seed PE from any prior history buffer
    {
      auto pts = history_.last(256);
      std::vector<double> cps;
      for (const auto& h : pts) if (h.multiplier > 0) cps.push_back(h.multiplier);
      if (!cps.empty()) n1_->seedHistory(cps);
    }

    
    if (config_.enablePollWorker) {
      PollWorker::Config pcfg;
      pcfg.intervalMs = config_.pollIntervalMs;
      auto hist = std::make_shared<CrashHistoryClient>(std::shared_ptr<HttpClient>(makeHttpClient(true)));
      poll_worker_ = std::make_unique<PollWorker>(std::move(hist), &router_, pcfg);
      poll_worker_->setHandler([this](const FetchedRound& fr) {
        CrashEvent ev;
        ev.kind = EventKind::End; ev.valid = true;
        ev.gameId = fr.gameId; ev.roundId = fr.gameId;
        ev.crashPoint = fr.multiplier; ev.hash = fr.hash;
        ev.beganAtMs = fr.beganAtMs; ev.endedAtMs = fr.crashedAtMs;
        ev.eventTimeMs = fr.crashedAtMs ? fr.crashedAtMs : nowMs();
        history_.pushFromEvent(ev);
        if (n1_) (void)n1_->onRoundEnd(ev);
      });
    }

    // ---- Persistence FIRST (required before LiveSupervisor / ColdStart / LiveBoot) ----
    if (config_.enablePersistence && secure_.database.valid()) {
      db_ = makeDatabase(secure_.database, true);
      auto cr = db_->connect();
      if (!cr) {
        log_.error("db connect failed (durable mode required): ", cr.error().message);
        return Error{cr.error().code, std::string("persistence connect failed: ") + cr.error().message};
      }
      MigrationRunner migrator(*db_);
      auto mr = migrator.applyAll();
      if (!mr) {
        log_.error("migration failed: ", mr.error().message);
        return Error{mr.error().code, std::string("migration failed: ") + mr.error().message};
      }
      batch_ = std::make_unique<BatchWriter>(db_.get());
      retention_ = std::make_unique<RetentionManager>(db_.get());
      outbox_repo_ = std::make_unique<OutboxRepository>(*db_);
      registry_.registerComponent(ComponentId::Persistence, "persistence", 6);
      durable_tx_ = std::make_unique<DurablePredictionTx>(db_.get(), outbox_repo_.get(), &handoff_, &outbox_);
      prediction_.setAdaptiveEdge(&adaptive_edge_);
      feedback_.setDatabase(db_.get());
      feedback_.setAdaptiveEdge(&adaptive_edge_);


      log_.info("persistence connected and migrations applied");
    } else if (config_.enablePersistence) {
      log_.error("enablePersistence=true but database config invalid");
      return Error{ErrorCode::ConfigError, "enablePersistence requires valid database config"};
    }

    // LiveSupervisor after DB is available
    live_supervisor_ = std::make_unique<LiveSupervisor>(db_.get());

    // Cold-start seeder — retained and invoked (no longer discarded temporary)
    cold_seeder_ = std::make_unique<ColdStartSeeder>(db_.get(), nullptr, nullptr, &history_);
    {
      auto seedR = cold_seeder_->seed();
      if (!seedR) {
        log_.warn("cold-start seed: ", seedR.error().message);
      } else {
        log_.info("cold-start seeded ", cold_seeder_->seededCount(), " rounds");
        auto pts = history_.last(256);
        std::vector<double> cps;
        for (const auto& h : pts) if (h.multiplier > 0) cps.push_back(h.multiplier);
        if (!cps.empty() && n1_) n1_->seedHistory(cps);
      }
    }

    // LiveBoot phases
    {
      LiveBoot::Deps deps;
      deps.supervisor = live_supervisor_.get();
      deps.seeder = cold_seeder_.get();
      deps.pollWorker = poll_worker_.get();
      deps.prewarmPrediction = [this]() {
        auto pts = history_.last(64);
        for (const auto& h : pts) {
          if (h.multiplier > 0) prediction_.engine().observe(h.multiplier, std::nullopt);
        }
      };
      live_boot_ = std::make_unique<LiveBoot>(std::move(deps));
      auto bootR = live_boot_->run();
      if (!bootR.ok) {
        log_.warn("LiveBoot incomplete: ", bootR.error);
      } else {
        log_.info("LiveBoot ok in ", bootR.durationMs, " ms");
      }
    }

    // Single authoritative response handler (Application owns completion).
    // Replaces any handler installed by PipelineLoop constructor.
    prediction_.setResponseHandler([this](const PredictionResponse& resp) {
      metric_reg_.counterInc(METRIC_PREDICTIONS_SUBMITTED);
      metrics_.inc(Metrics::PredictionsSubmitted);
      attempts_.complete(resp);
      if (!isActionable(resp)) {
        metric_reg_.counterInc(METRIC_PREDICTIONS_SKIPPED);
        return;
      }
      metrics_.inc(Metrics::PredictionsActionable);
      metric_reg_.counterInc(METRIC_PREDICTIONS_ACTIONABLE);
      auto pev = fromResponse(resp, resp.targetRoundId);
      // Register first; only claim ownership after successful registration
      auto reg = live_validator_.registerPrediction(pev);
      if (!reg) {
        log_.warn("registry: ", reg.error().message);
        return;
      }
      targets_.claim(resp.targetRoundId, pev.predictionId);
      targets_.markPredicted(resp.targetRoundId);
      (void)globalStats().notePrediction(true);
      auto sig = signalFromPrediction(pev);
      // Phase 1: durable TX (prediction + target_claims + outbox) then memory wake
      bool published = false;
      if (durable_tx_) {
        auto dr = durable_tx_->persistPrediction(pev, sig);
        if (dr.ok) {
          published = true;
          metrics_.inc(Metrics::OutboxPublished);
          metric_reg_.counterInc(METRIC_OUTBOX_PUBLISHED);
          forensics_.note(pev.predictionId, DeliveryStage::Published, pev.predictionId + "-pred");
          funnel_.notePublished();
          funnel_.noteActionable();
        } else {
          log_.warn("durable persist: ", dr.error);
          auto pub = handoff_.publishPrediction(sig);
          if (pub) {
            published = true;
            metrics_.inc(Metrics::OutboxPublished);
            metric_reg_.counterInc(METRIC_OUTBOX_PUBLISHED);
            forensics_.note(pev.predictionId, DeliveryStage::Published);
          }
        }
      } else {
        auto pub = handoff_.publishPrediction(sig);
        if (pub) {
          published = true;
          metrics_.inc(Metrics::OutboxPublished);
          metric_reg_.counterInc(METRIC_OUTBOX_PUBLISHED);
          forensics_.note(pev.predictionId, DeliveryStage::Published);
          funnel_.notePublished();
          funnel_.noteActionable();
        }
      }
      // If nothing was published, release target claim so a retry can succeed
      if (!published) {
        targets_.release(resp.targetRoundId);
        log_.warn("publish failed; released target ", resp.targetRoundId);
      }
    });

    live_validator_.setOutcomeHandler([this](const Outcome& o) {
      cooldown_.noteOutcome(o);
      invariants_.checkOutcome(o);
      if (o.isWin) {
        metrics_.inc(Metrics::OutcomesWin);
        globalStats().noteOutcome(true);
        metric_reg_.counterInc(METRIC_OUTCOMES_WIN);
      } else if (o.result == PredictionOutcome::Loss) {
        metrics_.inc(Metrics::OutcomesLoss);
        globalStats().noteOutcome(false);
        metric_reg_.counterInc(METRIC_OUTCOMES_LOSS);
      }
      targets_.markResolved(o.roundId);
      // Phase 3: exactly-once feedback → adaptive edge (deduped)
      if (feedback_.apply(o)) {
        forensics_.note(o.predictionId, DeliveryStage::Delivered);
      }
      auto sig = signalFromOutcome(o);
      if (durable_tx_) {
        auto dr = durable_tx_->persistOutcome(o, sig);
        if (dr.ok) {
          metrics_.inc(Metrics::OutboxPublished);
          metric_reg_.counterInc(METRIC_OUTBOX_PUBLISHED);
        } else {
          auto pub = handoff_.publishOutcome(std::move(sig));
          if (pub) {
            metrics_.inc(Metrics::OutboxPublished);
            metric_reg_.counterInc(METRIC_OUTBOX_PUBLISHED);
          }
        }
      } else {
        auto pub = handoff_.publishOutcome(std::move(sig));
        if (pub) {
          metrics_.inc(Metrics::OutboxPublished);
          metric_reg_.counterInc(METRIC_OUTBOX_PUBLISHED);
        }
      }
    });

    // Telegram + optional durable NotificationWorker
    if (config_.enableTelegram && secure_.telegram.valid()) {
      telegram_ = std::make_unique<TelegramClient>(secure_.telegram);
      dispatcher_ = std::make_unique<TelegramDispatcher>(
          secure_.telegram, secure_.telegram.extraChatIds);
      outbox_worker_ = std::make_unique<OutboxWorker>(outbox_,
          [this](const OutboxItem& item) {
            if (!telegram_) return false;
            // Phase 1: pre-send temporal authorization
            auto auth = temporal_.authorizeSend(item.signal.roundId, item.signal.createdAtMs);
            if (!auth.allowed()) {
              log_.warn("temporal reject: ", auth.reason, " round=", item.signal.roundId);
              metric_reg_.counterInc(METRIC_TELEGRAM_FAILED);
              return false; // treat as failed; lifecycle may dead-letter
            }
            auto text = SignalFormatter::formatTelegram(item.signal);
            auto r = telegram_->sendMessageWithRetry(text);
            if (r) metric_reg_.counterInc(METRIC_TELEGRAM_SENT);
            else metric_reg_.counterInc(METRIC_TELEGRAM_FAILED);
            return static_cast<bool>(r);
          });
      registry_.registerComponent(ComponentId::Telegram, "telegram", 5);
      if (outbox_repo_) {
        notification_worker_ = std::make_unique<NotificationWorker>(*outbox_repo_, telegram_.get());
        notification_worker_->setDeliverFn([this](const OutboxRow& row) -> Result<void> {
          // Pre-send temporal auth using round_id encoded in payload or row
          std::string rid = row.dedupeKey;
          auto pos = row.payload.find("round=");
          if (pos != std::string::npos) {
            auto end = row.payload.find('\n', pos);
            rid = row.payload.substr(pos + 6, end == std::string::npos ? std::string::npos : end - pos - 6);
          }
          auto auth = temporal_.authorizeSend(rid, row.createdAtMs);
          if (!auth.allowed()) {
            forensics_.note(row.id, DeliveryStage::Failed, row.id, auth.reason);
            return Error{ErrorCode::Timeout, auth.reason};
          }
          if (!telegram_) return Error{ErrorCode::ConfigError, "no telegram"};
          auto r = telegram_->sendMessageWithRetry(row.payload);
          if (!r) {
            forensics_.note(row.id, DeliveryStage::Failed, row.id, r.error().message);
            return Error{r.error().code, r.error().message};
          }
          forensics_.note(row.id, DeliveryStage::Delivered, row.id);
          metric_reg_.counterInc(METRIC_TELEGRAM_SENT);
          return Result<void>::success();
        });
        log_.info("NotificationWorker constructed (durable path + temporal auth)");
      }
    }

    registry_.setState(ComponentId::Application, ComponentState::Running);
    log_.info("CrashCore initialized");
    return Result<void>::success();
  }

  void start() {
    workers_.start();
    stats_server_ = std::make_unique<StatisticsServer>(config_.statsPort);
    stats_server_->start();
    log_.info("statistics HTTP server on :", config_.statsPort);
    if (live_supervisor_) {
      auto ar = live_supervisor_->acquire();
      if (!ar) log_.warn("supervisor acquire: ", ar.error().message);
      else live_supervisor_->startRenewLoop();
    }
    // poll worker may already have been started by LiveBoot; start only if not
    if (poll_worker_ && config_.enablePollWorker && !poll_worker_->running()) {
      poll_worker_->start();
      log_.info("poll worker started");
    }
    if (outbox_worker_) outbox_worker_->start();
    if (notification_worker_) {
      notification_worker_->start();
      log_.info("notification worker started");
    }
    if (batch_) batch_->start();
    if (native_socket_ && config_.enableNativeSocket) {
      auto r = native_socket_->start();
      if (!r) log_.warn("native socket start: ", r.error().message);
      else registry_.setState(ComponentId::Ingestion, ComponentState::Running);
    }
    running_.store(true);
    log_.info("CrashCore started");
  }

  void stop() {
    running_.store(false);
    // Bounded drain of in-flight events
    for (int i = 0; i < 20; ++i) {
      drainQueues();
      if (notification_worker_) (void)notification_worker_->tickOnce();
    }
    if (live_boot_) live_boot_->shutdown();
    if (notification_worker_) notification_worker_->stop();
    if (stats_server_) stats_server_->stop();
    if (poll_worker_) poll_worker_->stop();
    if (native_socket_) native_socket_->stop();
    if (outbox_worker_) outbox_worker_->stop();
    if (batch_) batch_->stop();
    if (live_supervisor_) {
      live_supervisor_->stopRenewLoop();
      live_supervisor_->release();
    }
    workers_.stop();
    registry_.setState(ComponentId::Application, ComponentState::Stopped);
    log_.info("CrashCore stopped");
  }

  Result<void> onBinaryFrame(const std::uint8_t* data, std::size_t len) {
    if (pipeline_) {
      // Pipeline owns decode/route; do NOT also injectFrame (duplicate path risk).
      auto r = pipeline_->onFrame(data, len);
      loop_cv_.notify_one(); // Phase 4: event-driven wake
      return r;
    }
    auto ev = decoder_.decodeBinary(data, len);
    if (!ev) return ev.error();
    metrics_.inc(Metrics::EventsDecoded);
    router_.route(std::move(ev.value()));
    metrics_.inc(Metrics::EventsRouted);
    drainQueues();
    return Result<void>::success();
  }

  void drainQueues() {
    if (pipeline_) {
      pipeline_->drain();
      return;
    }
    while (auto ev = router_.popPrediction()) {
      prediction_.submit(makePredictionRequest(*ev));
    }
    while (auto ev = router_.popValidation()) {
      history_.pushFromEvent(*ev);
      live_validator_.onGameEnd(*ev);
      if (n1_) {
        (void)n1_->onRoundEnd(*ev);
        globalStats().noteRound();
      }
    }
    attempts_.expireTimeouts();
    live_validator_.expireStale();
    outbox_life_.recoverExpiredLeases();
  }

  SystemHealth health() const {
    HealthAggregator agg;
    auto h = agg.collect(native_socket_.get(), &workers_, &outbox_, &registry_, &metrics_);
    // Phase 0: surface additional component readiness in summary
    std::ostringstream extra;
    extra << " db=" << (db_ ? "connected" : "absent");
    if (poll_worker_) extra << " poll=" << (poll_worker_->running() ? "running" : "stopped");
    if (outbox_worker_) extra << " outbox_worker=" << (outbox_worker_->running() ? "running" : "stopped");
    if (notification_worker_) extra << " notification_worker=" << (notification_worker_->running() ? "running" : "stopped");
    if (batch_) extra << " batch_writer=present";
    if (stats_server_) extra << " stats_server=" << (stats_server_->running() ? "running" : "stopped");
    if (live_supervisor_) extra << " supervisor=" << (live_supervisor_->hasAuthority() ? "authority" : "no_authority");
    if (live_boot_) extra << " live_boot=present";
    if (telegram_) extra << " telegram=present";
    extra << " feedback_applied=" << feedback_.appliedCount();
    extra << " feedback_dup=" << feedback_.duplicateCount();
    extra << " forensics=" << forensics_.eventCount();
    extra << " adaptive_edge=" << adaptive_edge_.currentEdge();
    extra << " " << funnel_.report();
    extra << " mean_skew_ms=" << skew_monitor_.meanSkewMs();
    if (skew_monitor_.isSkewed()) { h.healthy = false; extra << " SKEWED"; }
    h.summary += extra.str();
    return h;
  }

  std::string latencyBudgetReport() {
    LatencyBudgetMonitor mon(latency_);
    return mon.report();
  }

  bool latencyBudgetsOk() {
    LatencyBudgetMonitor mon(latency_);
    return mon.allOk();
  }

  bool running() const noexcept { return running_.load(); }
  EventDecoder& decoder() noexcept { return decoder_; }
  EventRouter& router() noexcept { return router_; }
  InProcessPredictionClient& prediction() noexcept { return prediction_; }
  N1Coordinator* n1() noexcept { return n1_.get(); }
  LiveValidator& liveValidator() noexcept { return live_validator_; }
  Outbox& outbox() noexcept { return outbox_; }
  LatencyTracker& latency() noexcept { return latency_; }
  Metrics& metrics() noexcept { return metrics_; }
  MetricRegistry& metricRegistry() noexcept { return metric_reg_; }
  PipelineLoop* pipeline() noexcept { return pipeline_.get(); }
  NativeBcGameSocket* nativeSocket() noexcept { return native_socket_.get(); }
  const ApplicationConfig& config() const noexcept { return config_; }
  ComponentRegistry& componentRegistry() noexcept { return registry_; }
  RollingHistoryBuffer& history() noexcept { return history_; }
  TargetCoordinator& targets() noexcept { return targets_; }
  LossCooldownTracker& cooldown() noexcept { return cooldown_; }

  int run() {
    auto ir = init();
    if (!ir) {
      log_.error("init failed: ", ir.error().message);
      return 1;
    }
    start();
    while (!Lifecycle::instance().shutdownRequested() && running()) {
      lag_monitor_.tick();
      drainQueues();
      live_validator_.expireStale();
      targets_.expireOlderThan(config_.stuckMaxAgeMs);
      outbox_life_.recoverExpiredLeases();
      if (retention_) retention_->pruneDatabase();
      // Phase 0.15: CV wait instead of fixed 5 ms sleep
      {
        std::unique_lock<std::mutex> lk(loop_mu_);
        loop_cv_.wait_for(lk, std::chrono::milliseconds(5), [&] {
          return Lifecycle::instance().shutdownRequested() || !running_.load();
        });
      }
    }
    stop();
    return 0;
  }

private:
  static void applyLogLevel(const std::string& level) {
    if (level == "debug") logger().setLevel(LogLevel::Debug);
    else if (level == "trace") logger().setLevel(LogLevel::Trace);
    else if (level == "warn") logger().setLevel(LogLevel::Warn);
    else if (level == "error") logger().setLevel(LogLevel::Error);
  }

  ApplicationConfig config_;
  std::unique_ptr<SecretProvider> secrets_;
  SecureConfig secure_;
  LatencyTracker latency_;
  Metrics metrics_;
  EventLoopLagMonitor lag_monitor_{5000}; // 5 ms expected tick
  MetricRegistry metric_reg_;
  EventDecoder decoder_;
  EventRouter router_;
  NativeSign sign_;
  Outbox outbox_;
  OutboxLifecycle outbox_life_;
  DurableHandoff handoff_;
  InProcessPredictionClient prediction_;
  GenerationFence fence_;
  PredictionAttemptCoordinator attempts_;
  LiveValidator live_validator_;
  WorkerPool workers_;
  ComponentRegistry registry_;
  RollingHistoryBuffer history_;
  TargetCoordinator targets_;
  LossCooldownTracker cooldown_;
  InvariantChecker invariants_;
  ClockOffset clock_offset_;
  SocketDiagnostics diagnostics_;
  std::unique_ptr<NativeBcGameSocket> native_socket_;
  std::unique_ptr<PipelineLoop> pipeline_;
  std::unique_ptr<TelegramClient> telegram_;
  std::unique_ptr<TelegramDispatcher> dispatcher_;
  std::unique_ptr<OutboxWorker> outbox_worker_;
  std::unique_ptr<Database> db_;
  std::unique_ptr<BatchWriter> batch_;
  std::unique_ptr<RetentionManager> retention_;
  std::unique_ptr<N1Coordinator> n1_;
  std::unique_ptr<StatisticsServer> stats_server_;
  std::unique_ptr<PollWorker> poll_worker_;
  std::unique_ptr<LiveBoot> live_boot_;
  std::unique_ptr<LiveSupervisor> live_supervisor_;
  std::unique_ptr<ColdStartSeeder> cold_seeder_;
  std::unique_ptr<OutboxRepository> outbox_repo_;
  std::unique_ptr<DurablePredictionTx> durable_tx_;
  std::unique_ptr<NotificationWorker> notification_worker_;
  TemporalGuard temporal_;
  acie::AdaptiveEdge adaptive_edge_;
  ExactlyOnceFeedback feedback_;
  DeliveryForensics forensics_;
  FunnelMetrics funnel_;
  ClockSkewMonitor skew_monitor_;
  StructuredLogger log_;
  std::atomic<bool> running_{false};
  std::mutex loop_mu_;
  std::condition_variable loop_cv_;
};

} // namespace crashcore
