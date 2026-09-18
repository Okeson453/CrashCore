#pragma once
/**
 * In-process PredictionClient backed by pe::PredictionEngine.
 * N+1 flow: observe(end of round N) → predict(target = N+1).
 *
 * Does not embed model math here — delegates to third_party/predictions-engine.
 */
#include "prediction_interface/prediction_client.hpp"
#include "prediction_interface/rolling_history.hpp"
#include "ingestion/crash_event.hpp"
#include "timing/timestamp.hpp"
#include "common/result.hpp"
#include "common/errors.hpp"

#include "pe/engine/prediction_engine.hpp"
#include "prediction/acie/strategy.hpp"
#include "prediction/acie/adaptive_edge.hpp"

#include <atomic>
#include <cmath>
#include <memory>
#include <mutex>
#include <optional>
#include <string>

namespace crashcore {

inline std::string nextRoundId(const std::string& currentGameId) {
  // Numeric game IDs: N → N+1. Non-numeric: append ".next"
  try {
    // Handle large IDs carefully
    std::size_t pos = 0;
    unsigned long long v = std::stoull(currentGameId, &pos);
    if (pos == currentGameId.size()) {
      return std::to_string(v + 1ULL);
    }
  } catch (...) {}
  return currentGameId + ".next";
}

/**
 * TestingEngine-aligned decision gate (quality mode skeleton).
 * For target T the fair probability is 1/T. Require modeled probability
 * to beat fair + configured edge, and confidence above minConf.
 * Default target 1.30 → fair ≈ 0.7692; with edge 0.02 the floor is ~0.789.
 */
inline PredictionDecision decisionFromSignal(const pe::PredictionSignal& sig,
                                             double minConf,
                                             double targetMult = 1.30,
                                             double qualityEdge = 0.02) {
  if (sig.probability <= 0.0 && sig.confidence <= 0.0) return PredictionDecision::Skip;
  if (!(targetMult > 1.0) || !std::isfinite(targetMult)) targetMult = 1.30;
  if (!(qualityEdge >= 0.0) || !std::isfinite(qualityEdge)) qualityEdge = 0.02;
  const double fair = 1.0 / targetMult;
  const double threshold = fair + qualityEdge;
  if (sig.probability >= threshold && sig.confidence >= minConf)
    return PredictionDecision::Enter;
  return PredictionDecision::Skip;
}

/**
 * InProcessPredictionClient — real PE integration.
 * Thread-safe: PE calls serialized by mutex (PE state is single-writer).
 */
class InProcessPredictionClient final : public PredictionClient {
public:
  struct Options {
    double minConfidence = 0.55;
    double defaultTarget = 1.30;
    double qualityEdge = 0.02;  // beat fair + edge (TestingEngine quality mode)
    double reducedEdge = 0.005;
    std::size_t warmMinSamples = 20;
    bool autoObserveOnSubmit = true;
    acie::StrategyMode strategyMode = acie::StrategyMode::Quality;
  };

  InProcessPredictionClient(PredictionContract contract, LatencyTracker* tracker)
      : InProcessPredictionClient(Options{}, std::move(contract), tracker) {}

  InProcessPredictionClient()
      : PredictionClient()
      , engine_(std::make_unique<pe::PredictionEngine>()) {}

  explicit InProcessPredictionClient(Options opt,
                                     PredictionContract contract = {},
                                     LatencyTracker* tracker = nullptr)
      : PredictionClient(std::move(contract), tracker)
      , opt_(opt)
      , engine_(std::make_unique<pe::PredictionEngine>()) {}

  pe::PredictionEngine& engine() noexcept { return *engine_; }
  const pe::PredictionEngine& engine() const noexcept { return *engine_; }

  /** Feed a finished round into PE state (O(1)). Call on every End. */
  void observeEnd(const CrashEvent& endEv) {
    if (endEv.kind != EventKind::End || !endEv.valid) return;
    const double cp = endEv.crashPoint > 0 ? endEv.crashPoint : endEv.currentMult;
    if (!(cp > 0.0) || !std::isfinite(cp)) return;
    std::lock_guard lk(pe_mu_);
    std::optional<std::int64_t> began;
    if (endEv.beganAtMs > 0) began = endEv.beganAtMs;
    engine_->observe(cp, began);
    history_.pushFromEvent(endEv);
    last_observed_round_ = endEv.roundId.empty() ? endEv.gameId : endEv.roundId;
    ++observed_;
  }

  /** N+1 predict after an End: observe N, then predict for N+1. */
  Result<PredictionResponse> predictNextAfterEnd(const CrashEvent& endEv) {
    observeEnd(endEv);
    PredictionRequest req;
    req.gameId = endEv.gameId;
    req.targetRoundId = nextRoundId(endEv.roundId.empty() ? endEv.gameId : endEv.roundId);
    req.requestTimeMs = nowMs();
    req.requestNs = steadyNs();
    req.sequence = endEv.sequence + 1;
    req.currentMult = endEv.crashPoint;
    req.correlationId = req.targetRoundId + "-n1-" + std::to_string(req.requestTimeMs);
    auto r = submit(req);
    if (!r) return r.error();
    auto polled = pollResponse(req.correlationId);
    if (!polled) {
      // handler already called; reconstruct from last
      return last_response_;
    }
    return *polled;
  }

  Result<void> submit(const PredictionRequest& req) override {
    const auto t0 = steadyNs();
    PredictionResponse resp;
    resp.correlationId = req.correlationId;
    resp.targetRoundId = req.targetRoundId;
    resp.responseTimeMs = nowMs();
    resp.responseNs = steadyNs();

    try {
      pe::PredictRequest preq;
      preq.targetRoundId = req.targetRoundId;
      preq.target = opt_.defaultTarget;
      if (req.currentMult > 1.0 && req.currentMult < 100.0) {
        // optional: use currentMult as soft target bias — PE uses ThresholdTarget
      }

      pe::PredictResult pr;
      {
        std::lock_guard lk(pe_mu_);
        // Warm path: if history has points and PE is cold, seed from rolling history
        if (engine_->state().getLifecycleState() == pe::EngineLifecycleState::COLD) {
          auto pts = history_.last(256);
          for (const auto& hr : pts) {
            if (hr.multiplier > 0) engine_->observe(hr.multiplier, std::nullopt);
          }
        }
        pr = engine_->predict(preq);
      }

      resp.predictionId = pr.output.predictionId.empty()
                              ? ("pe-" + req.correlationId)
                              : pr.output.predictionId;
      resp.confidence = pr.signal.confidence;
      resp.probability = pr.signal.probability;
      resp.targetMult = static_cast<double>(pr.output.target);
      resp.modelVersion = pr.signal.modelVersion.empty()
                              ? pr.output.model.name
                              : pr.signal.modelVersion;
      {
        acie::StrategyConfig scfg;
        scfg.mode = opt_.strategyMode;
        scfg.defaultTarget = opt_.defaultTarget;
        scfg.qualityEdge = adaptive_edge_ ? adaptive_edge_->currentEdge() : opt_.qualityEdge;
        scfg.reducedEdge = opt_.reducedEdge;
        scfg.minConfidence = opt_.minConfidence;
        scfg.warmMinSamples = opt_.warmMinSamples;
        acie::StrategyInput sin;
        sin.probability = pr.signal.probability;
        sin.confidence = pr.signal.confidence;
        sin.targetMult = opt_.defaultTarget;
        sin.samplesSeen = static_cast<std::size_t>(observed_.load());
        sin.modelStale = false;
        sin.entitlementOk = true;
        auto sres = acie::evaluateStrategy(sin, scfg);
        resp.decision = sres.decision;
        // Keep legacy gate as safety net alignment check (fair-odds)
        if (resp.decision == PredictionDecision::Enter) {
          auto legacy = decisionFromSignal(pr.signal, opt_.minConfidence,
                                           opt_.defaultTarget, scfg.qualityEdge);
          if (legacy == PredictionDecision::Skip) resp.decision = PredictionDecision::Skip;
        }
      }
      resp.valid = true;
      ++predicted_;
      if (resp.decision == PredictionDecision::Enter) ++actionable_;
    } catch (const pe::PipelineStageError& e) {
      resp.valid = true;
      resp.decision = PredictionDecision::Skip;
      resp.confidence = 0.0;
      resp.modelVersion = std::string("pe-error:") + pe::toString(e.stage);
      resp.predictionId = "skip-" + req.correlationId;
      ++errors_;
    } catch (const std::exception& e) {
      resp.valid = true;
      resp.decision = PredictionDecision::Skip;
      resp.confidence = 0.0;
      resp.modelVersion = std::string("pe-error:") + e.what();
      resp.predictionId = "skip-" + req.correlationId;
      ++errors_;
    }

    {
      std::lock_guard lk(mu_);
      pending_[req.correlationId] = resp;
      last_response_ = resp;
    }
    ++submitted_;
    if (tracker_) {
      tracker_->record(LatencyTracker::Stage::Ipc, (steadyNs() - t0) / 1000);
    }
    if (handler_) handler_(resp);
    return Result<void>::success();
  }

  std::uint64_t observedCount() const noexcept { return observed_.load(); }
  std::uint64_t predictedCount() const noexcept { return predicted_.load(); }
  std::uint64_t actionableCount() const noexcept { return actionable_.load(); }
  std::uint64_t errorCount() const noexcept { return errors_.load(); }
  std::string lastObservedRound() const {
    std::lock_guard lk(pe_mu_);
    return last_observed_round_;
  }

  RollingHistoryBuffer& history() noexcept { return history_; }

  void setAdaptiveEdge(acie::AdaptiveEdge* edge) noexcept { adaptive_edge_ = edge; }
  acie::AdaptiveEdge* adaptiveEdge() noexcept { return adaptive_edge_; }
  void setOptions(const Options& o) noexcept { opt_ = o; }
  const Options& options() const noexcept { return opt_; }

private:
  Options opt_;
  std::unique_ptr<pe::PredictionEngine> engine_;
  mutable std::mutex pe_mu_;
  RollingHistoryBuffer history_{256};
  std::string last_observed_round_;
  PredictionResponse last_response_;
  std::atomic<std::uint64_t> observed_{0}, predicted_{0}, actionable_{0}, errors_{0};
  acie::AdaptiveEdge* adaptive_edge_ = nullptr;
};

} // namespace crashcore
