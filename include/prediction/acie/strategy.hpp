#pragma once
/**
 * Phase 2 — ACIE strategy / decision surface (TestingEngine-aligned skeleton).
 *
 * Modes:
 *   ENTRY         — full signal (probability beats fair + qualityEdge, conf ok)
 *   REDUCED_ENTRY — weaker edge / HF path (probability beats fair + reducedEdge)
 *   SKIP          — below thresholds or cold/stale
 *
 * This is the decision contract layer. Full PSI ensemble / model registry
 * remains Phase 2+; the gate itself is authoritative for Enter vs Skip.
 */
#include "prediction_interface/prediction_response.hpp"
#include "common/types.hpp"
#include <algorithm>
#include <cmath>
#include <string>

namespace crashcore {
namespace acie {

enum class StrategyMode : std::uint8_t {
  Quality = 0,   // fair + qualityEdge (default production)
  HighFrequency, // fair + reducedEdge, lower conf floor
  Conservative   // fair + qualityEdge + extra margin
};

enum class StrategyAction : std::uint8_t {
  Skip = 0,
  ReducedEntry,
  Entry
};

struct StrategyConfig {
  StrategyMode mode = StrategyMode::Quality;
  double defaultTarget = 1.30;
  double qualityEdge = 0.02;
  double reducedEdge = 0.005;
  double conservativeExtra = 0.03;
  double minConfidence = 0.55;
  double reducedMinConfidence = 0.45;
  std::size_t warmMinSamples = 20;
};

struct StrategyInput {
  double probability = 0.0;
  double confidence = 0.0;
  double targetMult = 1.30;
  std::size_t samplesSeen = 0;
  bool modelStale = false;
  bool entitlementOk = true;
};

struct StrategyResult {
  StrategyAction action = StrategyAction::Skip;
  PredictionDecision decision = PredictionDecision::Skip;
  double fairProbability = 0.0;
  double requiredProbability = 0.0;
  double edgeUsed = 0.0;
  std::string reason;
};

inline double fairProbability(double targetMult) {
  if (!(targetMult > 1.0) || !std::isfinite(targetMult)) return 1.0 / 1.30;
  return 1.0 / targetMult;
}

/**
 * Core ACIE-style strategy evaluation.
 */
inline StrategyResult evaluateStrategy(const StrategyInput& in,
                                       const StrategyConfig& cfg = {}) {
  StrategyResult r;
  r.fairProbability = fairProbability(in.targetMult > 1.0 ? in.targetMult : cfg.defaultTarget);

  if (!in.entitlementOk) {
    r.reason = "entitlement_denied";
    return r;
  }
  if (in.modelStale) {
    r.reason = "model_stale";
    return r;
  }
  if (in.samplesSeen < cfg.warmMinSamples) {
    r.reason = "cold_start";
    return r;
  }
  if (in.probability <= 0.0 && in.confidence <= 0.0) {
    r.reason = "zero_signal";
    return r;
  }

  double edge = cfg.qualityEdge;
  double minConf = cfg.minConfidence;
  switch (cfg.mode) {
    case StrategyMode::HighFrequency:
      edge = cfg.reducedEdge;
      minConf = cfg.reducedMinConfidence;
      break;
    case StrategyMode::Conservative:
      edge = cfg.qualityEdge + cfg.conservativeExtra;
      break;
    case StrategyMode::Quality:
    default:
      break;
  }
  r.edgeUsed = edge;
  r.requiredProbability = r.fairProbability + edge;

  if (in.confidence < minConf) {
    r.reason = "low_confidence";
    return r;
  }

  if (in.probability >= r.requiredProbability) {
    r.action = StrategyAction::Entry;
    r.decision = PredictionDecision::Enter;
    r.reason = "entry_quality";
    return r;
  }

  // Reduced entry: beat fair + reducedEdge with reduced conf (HF / partial)
  const double reducedReq = r.fairProbability + cfg.reducedEdge;
  if (cfg.mode != StrategyMode::Conservative &&
      in.probability >= reducedReq &&
      in.confidence >= cfg.reducedMinConfidence) {
    r.action = StrategyAction::ReducedEntry;
    r.decision = PredictionDecision::Enter; // still actionable; size reduced downstream
    r.edgeUsed = cfg.reducedEdge;
    r.requiredProbability = reducedReq;
    r.reason = "reduced_entry";
    return r;
  }

  r.reason = "below_edge";
  return r;
}

/** Map StrategyAction → PredictionDecision for existing clients. */
inline PredictionDecision toDecision(StrategyAction a) {
  switch (a) {
    case StrategyAction::Entry:
    case StrategyAction::ReducedEntry:
      return PredictionDecision::Enter;
    default:
      return PredictionDecision::Skip;
  }
}

} // namespace acie
} // namespace crashcore
