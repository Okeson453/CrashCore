#pragma once
/**
 * Core event-processing loop concept mapped from TestingEngine live supervisor
 * + realtime pipeline + validation + outbox wake.
 *
 * Hot path: frame → decode → normalize → validate → route → prediction iface
 * Cold path: outcomes → feedback → outbox → telegram / persistence
 */
#include "ingestion/event_decoder.hpp"
#include "ingestion/event_router.hpp"
#include "ingestion/round_state.hpp"
#include "ingestion/duplicate_filter.hpp"
#include "ingestion/stats.hpp"
#include "realtime/realtime_pipeline.hpp"
#include "prediction_interface/prediction_client.hpp"
#include "prediction_interface/prediction_attempt.hpp"
#include "validation/live_validator.hpp"
#include "validation/fencing.hpp"
#include "delivery/outbox/outbox_lifecycle.hpp"
#include "delivery/signal.hpp"
#include "timing/latency_tracker.hpp"
#include "timing/clock_offset.hpp"
#include "telemetry/metrics.hpp"
#include "common/result.hpp"
#include <atomic>
#include <functional>
#include <string>

namespace crashcore {

class PipelineLoop {
public:
  /**
   * @param installHandlers When false (Application-owned mode), do not install
   *        response/outcome handlers — Application is the single authority.
   *        When true (standalone/tests), PipelineLoop owns handlers.
   */
  PipelineLoop(EventDecoder& decoder,
               EventRouter& router,
               PredictionClient& predClient,
               LiveValidator& liveValidator,
               OutboxLifecycle& outboxLife,
               LatencyTracker* tracker = nullptr,
               bool installHandlers = true)
      : decoder_(decoder)
      , router_(router)
      , pred_(predClient)
      , validator_(liveValidator)
      , outbox_(outboxLife)
      , tracker_(tracker)
      , fence_()
      , attempts_(predClient, fence_, tracker)
      , realtime_()
      , rounds_()
      , dupes_()
      , stats_()
      , clock_()
      , metrics_()
      , ownsHandlers_(installHandlers) {
    if (installHandlers) {
      pred_.setResponseHandler([this](const PredictionResponse& resp) {
        onPredictionResponse(resp);
      });
      validator_.setOutcomeHandler([this](const Outcome& o) {
        onOutcome(o);
      });
    }
    realtime_.setAcceptedHandler([this](const realtime::ValidatedRoundEvent& ev) {
      onValidated(ev);
    });
  }

  /** Single-frame intake — primary loop iteration body. */
  Result<void> onFrame(const std::uint8_t* data, std::size_t len) {
    const auto t0 = steadyNs();
    metrics_.inc(Metrics::EventsReceived);

    auto decoded = decoder_.decodeBinary(data, len);
    if (!decoded) {
      metrics_.inc(Metrics::EventsDropped);
      return decoded.error();
    }
    auto ev = std::move(decoded.value());
    metrics_.inc(Metrics::EventsDecoded);

    // Realtime normalize/validate path
    auto accepted = realtime_.observe(ev);
    if (!accepted) {
      // still allow heartbeat/connect through without round identity
      if (ev.kind == EventKind::Heartbeat || ev.kind == EventKind::Connect) {
        return Result<void>::success();
      }
      // If realtime rejected as invalid but decoder marked valid, still try route
      if (!ev.valid) return Result<void>::success();
    }

    if (!dupes_.accept(ev)) {
      metrics_.inc(Metrics::EventsDropped);
      return Result<void>::success();
    }

    rounds_.apply(ev);

    if (ev.kind == EventKind::End && ev.crashPoint > 0) {
      stats_.addRound(ev.crashPoint);
    }

    if (!router_.route(ev)) {
      metrics_.inc(Metrics::EventsDropped);
    } else {
      metrics_.inc(Metrics::EventsRouted);
    }

    drain();

    if (tracker_) {
      tracker_->record(LatencyTracker::Stage::Total, (steadyNs() - t0) / 1000);
    }
    return Result<void>::success();
  }

  void drain() {
    while (auto ev = router_.popPrediction()) {
      auto req = makePredictionRequest(*ev);
      attempts_.begin(req);
      // Also submit for stub client path
      pred_.submit(req);
      metrics_.inc(Metrics::PredictionsSubmitted);
    }
    while (auto ev = router_.popValidation()) {
      auto result = validator_.onGameEnd(*ev);
      (void)result;
    }
    attempts_.expireTimeouts();
    validator_.expireStale();
    outbox_.recoverExpiredLeases();
  }

  void onPredictionResponse(const PredictionResponse& resp) {
    attempts_.complete(resp);
    if (!isActionable(resp)) return;
    metrics_.inc(Metrics::PredictionsActionable);
    auto pev = fromResponse(resp, resp.targetRoundId);
    auto reg = validator_.registerPrediction(pev);
    if (!reg) return;
    auto sig = signalFromPrediction(pev);
    outbox_.publishSignal(std::move(sig));
    metrics_.inc(Metrics::OutboxPublished);
  }

  void onOutcome(const Outcome& o) {
    if (o.isWin) metrics_.inc(Metrics::OutcomesWin);
    else if (o.result == PredictionOutcome::Loss) metrics_.inc(Metrics::OutcomesLoss);
    auto sig = signalFromOutcome(o);
    outbox_.publishSignal(std::move(sig));
    metrics_.inc(Metrics::OutboxPublished);
  }

  void onValidated(const realtime::ValidatedRoundEvent&) {
    // hook for metrics / tracing
  }

  RoundStateRegistry& rounds() noexcept { return rounds_; }
  CrashStatsAccumulator& stats() noexcept { return stats_; }
  realtime::RealtimePipeline& realtime() noexcept { return realtime_; }
  PredictionAttemptCoordinator& attempts() noexcept { return attempts_; }
  Metrics& metrics() noexcept { return metrics_; }
  ClockOffset& clock() noexcept { return clock_; }
  GenerationFence& fence() noexcept { return fence_; }

private:
  EventDecoder& decoder_;
  EventRouter& router_;
  PredictionClient& pred_;
  LiveValidator& validator_;
  OutboxLifecycle& outbox_;
  LatencyTracker* tracker_;
  GenerationFence fence_;
  PredictionAttemptCoordinator attempts_;
  realtime::RealtimePipeline realtime_;
  RoundStateRegistry rounds_;
  DuplicateFilter dupes_;
  CrashStatsAccumulator stats_;
  ClockOffset clock_;
  Metrics metrics_;
  bool ownsHandlers_ = true;
};

} // namespace crashcore
