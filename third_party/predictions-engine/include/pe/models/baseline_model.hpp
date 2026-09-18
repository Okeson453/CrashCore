#pragma once

/**
 * Baseline statistical model — honest empirical rates + constrained adjustments.
 * Ported from baseline-model.ts (core logic; adaptive boosts gated by flag).
 */

#include "pe/features/feature_meta.hpp"
#include "pe/models/predictive_model.hpp"
#include "pe/util/time.hpp"
#include "pe/util/uuid.hpp"

#include <algorithm>
#include <cmath>
#include <deque>
#include <string>
#include <unordered_map>
#include <vector>

namespace pe {

/** Approximate long-run P(crash ≥ 1.3) under fair crash process ≈ 1/1.3. */
inline constexpr double EMPIRICAL_BASE_1_30 = 1.0 / 1.3;

struct OutcomeSample {
  double predicted = 0.0;
  int actual = 0; // 0 | 1
  bool gapActive = false;
  bool streakActive = false;
  bool anomalyActive = false;
};

struct BaselineAdaptiveState {
  int version = 1;
  double gapMultiplier = 1.0;
  double streakMultiplier = 1.0;
  double anomalyMultiplier = 1.0;
  double shortWeight = 0.4;
  double midWeight = 0.35;
  double longWeight = 0.25;
  std::deque<OutcomeSample> outcomes;
  double rollingAbsError = 0.15;
  bool allowHeuristicBoosts = false; // deliberate: gated (deviation from ungated TS bug)
  std::string updatedAt;
};

class BaselineStatisticalModel : public PredictiveModel {
public:
  BaselineStatisticalModel() {
    identity_ = {
      "baseline-statistical",
      "1.2.0",
      FEATURE_VERSION_V2,
      "targets-v1"
    };
    adaptive_.updatedAt = util::nowIso8601();
  }

  const ModelIdentity& identity() const override { return identity_; }

  PredictionOutput predict(
      const FeatureVector& features,
      ThresholdTarget target,
      const Regime* regime) const override {
    const auto& v = features.values;
    auto get = [&](const std::string& key, double def = 0.0) -> double {
      auto it = v.find(key);
      return it != v.end() ? it->second : def;
    };

    // Window hit rates (genuine, not SHORT_CAP aliases)
    const double w20  = get("hit_1_30_20", get("hit_13", EMPIRICAL_BASE_1_30));
    const double w50  = get("hit_1_30_50", w20);
    const double w100 = get("hit_1_30_100", w50);
    const double w200 = get("hit_1_30_200", w100);
    const double ewma = get("ewma_hit_13", EMPIRICAL_BASE_1_30);

    // Blend
    double p = adaptive_.shortWeight * w20
             + adaptive_.midWeight * w50
             + adaptive_.longWeight * std::max(w100, w200 * 0.95);
    // Soft shrink toward empirical base when thin or poor calibration
    const double sampleSize = get("sample_size", 0.0);
    const double shrink = sampleSize < 50.0
        ? 0.55
        : (adaptive_.rollingAbsError > 0.12 ? 0.35 : 0.15);
    p = (1.0 - shrink) * p + shrink * EMPIRICAL_BASE_1_30;

    // Gated heuristic boosts (deliberate deviation: TS had ungated boost in regime_adjusted)
    if (adaptive_.allowHeuristicBoosts) {
      p *= adaptive_.gapMultiplier;
      p *= adaptive_.streakMultiplier;
      p *= adaptive_.anomalyMultiplier;
    }

    // Target scaling: higher thresholds → lower probability
    if (target >= 10.0) p *= 0.08;
    else if (target >= 5.0) p *= 0.22;
    else if (target >= 2.0) p *= 0.55;
    // else 1.3 keeps p

    p = std::clamp(p, 0.01, 0.99);

    // Confidence: calibration-aware, never claims ~100%
    const double quality = get("quality_score", 0.5);
    double conf = 0.55 * quality + 0.25 * (1.0 - std::min(1.0, adaptive_.rollingAbsError / 0.2))
                + 0.20 * std::min(1.0, sampleSize / 100.0);
    if (regime && regime->dimensions.anomalyState) conf *= 0.7;
    conf = std::clamp(conf, 0.15, 0.92);

    PredictionOutput out;
    out.predictionId = util::randomUUID();
    out.model = identity_;
    out.target = target;
    out.score = p; // score == probability for baseline
    out.probability = p;
    out.confidence = conf;
    if (regime) out.regime = *regime;
    out.dataQuality = quality;
    out.featureSummary = {
      {"w20", w20}, {"w50", w50}, {"w100", w100}, {"w200", w200},
      {"ewma_hit_13", ewma}, {"blend_p", p}, {"shrink", shrink}
    };
    out.reasoning = {
      "baseline-statistical v1.2.0",
      "window blend + soft shrink toward empirical base rate",
      adaptive_.allowHeuristicBoosts ? "heuristic boosts enabled" : "heuristic boosts gated off"
    };
    if (regime) out.reasoning.push_back("regime=" + regime->id);
    out.timestamp = util::nowIso8601();
    out.expiresAt = util::expiresAtIso(30);
    return out;
  }

  void observeOutcome(double predicted, int actual, double /*crashPoint*/ = 0.0,
                      ThresholdTarget /*target*/ = 1.3) override {
    OutcomeSample s;
    s.predicted = predicted;
    s.actual = actual;
    adaptive_.outcomes.push_back(s);
    if (adaptive_.outcomes.size() > 200) adaptive_.outcomes.pop_front();

    // Rolling abs error
    double sum = 0.0;
    for (const auto& o : adaptive_.outcomes)
      sum += std::abs(o.predicted - static_cast<double>(o.actual));
    adaptive_.rollingAbsError = sum / static_cast<double>(adaptive_.outcomes.size());
    adaptive_.updatedAt = util::nowIso8601();
  }

  BaselineAdaptiveState& adaptiveState() { return adaptive_; }
  const BaselineAdaptiveState& adaptiveState() const { return adaptive_; }

  void setAllowHeuristicBoosts(bool v) { adaptive_.allowHeuristicBoosts = v; }

private:
  ModelIdentity identity_;
  mutable BaselineAdaptiveState adaptive_;
};

} // namespace pe
