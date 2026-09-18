#pragma once
/**
 * N1Coordinator — End(N) → observe → predict(N+1) → register/handoff.
 * Wires PE into the live pipeline without embedding model logic.
 */
#include "prediction_interface/in_process_prediction_client.hpp"
#include "validation/live_validator.hpp"
#include "delivery/outbox/durable_handoff.hpp"
#include "delivery/signal.hpp"
#include "prediction_interface/prediction_event.hpp"
#include "prediction_interface/prediction_response.hpp"
#include <cmath>
#include "ingestion/crash_event.hpp"
#include "telemetry/metrics.hpp"
#include "timing/latency_tracker.hpp"
#include "common/result.hpp"
#include <atomic>
#include <functional>
#include <memory>

namespace crashcore {

class N1Coordinator {
public:
  using PredictionPublished = std::function<void(const PredictionEvent&)>;

  N1Coordinator(InProcessPredictionClient& client,
                LiveValidator& validator,
                DurableHandoff* handoff = nullptr,
                Metrics* metrics = nullptr,
                LatencyTracker* tracker = nullptr)
      : client_(client)
      , validator_(validator)
      , handoff_(handoff)
      , metrics_(metrics)
      , tracker_(tracker) {}

  void setOnPublished(PredictionPublished h) { on_published_ = std::move(h); }

  /**
   * Handle a validated End event: observe crash into PE, predict for N+1.
   * Returns the prediction response for N+1 (may be Skip).
   */
  Result<PredictionResponse> onRoundEnd(const CrashEvent& endEv) {
    if (endEv.kind != EventKind::End || !endEv.valid) {
      return Error{ErrorCode::InvalidArgument, "N1 requires valid End event"};
    }
    const auto t0 = steadyNs();
    auto r = client_.predictNextAfterEnd(endEv);
    if (!r) {
      ++failures_;
      return r;
    }
    auto resp = r.value();
    ++n1_attempts_;

    if (isActionable(resp)) {
      auto pev = fromResponse(resp, endEv.gameId, endEv.crashPoint);
      auto reg = validator_.registerPrediction(pev);
      if (reg) {
        ++registered_;
        if (handoff_) {
          auto sig = signalFromPrediction(pev);
          handoff_->publishPrediction(std::move(sig));
        }
        if (on_published_) on_published_(pev);
      }
      ++actionable_;
    } else {
      ++skipped_;
    }

    if (metrics_) {
      metrics_->inc(Metrics::PredictionsSubmitted);
      if (isActionable(resp)) metrics_->inc(Metrics::PredictionsActionable);
    }
    if (tracker_) {
      tracker_->record(LatencyTracker::Stage::Prediction, (steadyNs() - t0) / 1000);
    }
    return resp;
  }

  /** Seed PE from historical crash points (cold start). */
  void seedHistory(const std::vector<double>& crashPoints) {
    for (double cp : crashPoints) {
      if (cp > 0 && std::isfinite(cp)) {
        CrashEvent ev;
        ev.kind = EventKind::End;
        ev.valid = true;
        ev.crashPoint = cp;
        ev.gameId = "seed";
        ev.roundId = "seed";
        client_.observeEnd(ev);
      }
    }
  }

  std::uint64_t n1Attempts() const noexcept { return n1_attempts_.load(); }
  std::uint64_t actionableCount() const noexcept { return actionable_.load(); }
  std::uint64_t skippedCount() const noexcept { return skipped_.load(); }
  std::uint64_t registeredCount() const noexcept { return registered_.load(); }
  std::uint64_t failureCount() const noexcept { return failures_.load(); }

private:
  InProcessPredictionClient& client_;
  LiveValidator& validator_;
  DurableHandoff* handoff_ = nullptr;
  Metrics* metrics_ = nullptr;
  LatencyTracker* tracker_ = nullptr;
  PredictionPublished on_published_;
  std::atomic<std::uint64_t> n1_attempts_{0}, actionable_{0}, skipped_{0}, registered_{0}, failures_{0};
};

} // namespace crashcore
