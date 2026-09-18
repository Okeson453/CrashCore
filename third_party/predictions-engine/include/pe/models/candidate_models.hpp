#pragma once

/**
 * Candidate sequence models + pure-function helpers.
 * Complete port of candidate-models.ts + thin wrappers.
 * Self-contained — no imports outside pe/.
 */

#include "pe/features/feature_meta.hpp"
#include "pe/models/baseline_model.hpp"
#include "pe/models/predictive_model.hpp"
#include "pe/state/incremental_state_engine.hpp"
#include "pe/util/time.hpp"
#include "pe/util/uuid.hpp"

#include <algorithm>
#include <cmath>
#include <optional>
#include <string>
#include <vector>

namespace pe {

struct CandidateEstimate {
  std::string modelName;
  double probability = 0.0;
};

inline double clamp01(double x) {
  return std::max(0.01, std::min(0.99, x));
}

inline double frequencyModelProbability(double ewmaHitRate, double fallback = 0.65) {
  return ewmaHitRate > 0.0 ? clamp01(ewmaHitRate) : fallback;
}

inline double streakAwareProbability(double baseline, std::optional<double> streakHitRate) {
  if (!streakHitRate) return clamp01(baseline);
  return clamp01(*streakHitRate);
}

inline double conditionalFrequencyProbability(double conditional, double baseline,
                                              int matchCount, int minMatches = 40) {
  const double p = matchCount >= minMatches ? conditional : baseline;
  return clamp01(p);
}

/**
 * Deliberate: gated behind allowBoost (TS applied ungated boost — we gate it).
 */
inline double regimeAdjustedProbability(double baseline, const std::string& regime,
                                        double improvement, bool allowBoost = false) {
  double p = baseline;
  if (allowBoost) {
    if (regime == "low-cluster" || regime == "deep-low")
      p = baseline + std::max(0.02, improvement * 0.55);
    else if (regime == "volatile")
      p = baseline * 0.97;
    else if (regime == "high-activity" || regime == "high-burst")
      p = baseline + 0.025;
  }
  return clamp01(p);
}

/** Lag-1 autocorrelation adjusted frequency */
inline CandidateEstimate autocorrelationModel(const IncrementalStateEngine& engine) {
  const double baseline = engine.ewmaHit13();
  const auto lags = engine.getLagArray();
  const int n = static_cast<int>(lags.size());
  if (n < 16) return {"AutocorrelationModel", clamp01(baseline)};
  double mean = 0.0;
  for (double x : lags) mean += (x >= 1.3 ? 1.0 : 0.0);
  mean /= n;
  double num = 0.0, den = 0.0;
  for (int i = 1; i < n; ++i) {
    const double a = (lags[static_cast<size_t>(i)] >= 1.3 ? 1.0 : 0.0) - mean;
    const double b = (lags[static_cast<size_t>(i - 1)] >= 1.3 ? 1.0 : 0.0) - mean;
    num += a * b;
    den += b * b;
  }
  const double acf = den > 1e-9 ? num / den : 0.0;
  const double last = lags[static_cast<size_t>(n - 1)] >= 1.3 ? 1.0 : 0.0;
  const double adj = baseline + acf * (last - baseline) * 0.15;
  return {"AutocorrelationModel", clamp01(adj)};
}

inline CandidateEstimate markovChainModel(const IncrementalStateEngine& engine) {
  return {"MarkovChainModel", clamp01(engine.markovPNextAbove13())};
}

inline CandidateEstimate spectralModel(const IncrementalStateEngine& engine) {
  const double baseline = engine.ewmaHit13();
  const auto lags = engine.getLagArray();
  if (static_cast<int>(lags.size()) < 16)
    return {"SpectralModel", clamp01(baseline)};
  double mean = 0.0;
  for (double x : lags) mean += x;
  mean /= static_cast<double>(lags.size());
  double energy = 0.0;
  for (double x : lags) { const double d = x - mean; energy += d * d; }
  const double shortR = engine.shortHitRate13();
  const double mix = std::min(0.2, energy / (static_cast<double>(lags.size()) * 50.0 + 1e-9));
  return {"SpectralModel", clamp01((1.0 - mix) * baseline + mix * shortR)};
}

inline CandidateEstimate entropyModel(const IncrementalStateEngine& engine) {
  const double baseline = engine.ewmaHit13();
  const double p = engine.shortHitRate13();
  const double q = std::min(0.999, std::max(0.001, p));
  const double entropy = -(q * std::log2(q) + (1.0 - q) * std::log2(1.0 - q));
  const double certainty = 1.0 - entropy;
  return {"EntropyModel", clamp01((1.0 - certainty * 0.3) * baseline + certainty * 0.3 * p)};
}

// --- Thin PredictiveModel adapters ---

namespace detail {
inline PredictionOutput makeSimpleOut(const ModelIdentity& id, double p, ThresholdTarget target,
                                      const Regime* regime, double dq,
                                      const std::vector<std::string>& reason) {
  PredictionOutput out;
  out.predictionId = util::randomUUID();
  out.model = id;
  out.target = target;
  out.score = p;
  out.probability = p;
  out.confidence = 0.5;
  if (regime) out.regime = *regime;
  out.dataQuality = dq;
  out.reasoning = reason;
  out.timestamp = util::nowIso8601();
  out.expiresAt = util::expiresAtIso(30);
  return out;
}
} // namespace detail

class FrequencyModel : public PredictiveModel {
public:
  FrequencyModel() { identity_ = {"frequency", "1.0.0", FEATURE_VERSION_V2, "targets-v1"}; }
  const ModelIdentity& identity() const override { return identity_; }
  PredictionOutput predict(const FeatureVector& features, ThresholdTarget target,
                           const Regime* regime) const override {
    const double ewma = features.values.count("ewma_hit_13") ? features.values.at("ewma_hit_13") : 0.65;
    double p = frequencyModelProbability(ewma);
    if (target >= 2.0) p *= 0.55;
    if (target >= 5.0) p *= 0.4;
    return detail::makeSimpleOut(identity_, clamp01(p), target, regime,
                                 features.meta.dataQualityScore, {"frequency-model"});
  }
private:
  ModelIdentity identity_;
};

class StreakAwareModel : public PredictiveModel {
public:
  StreakAwareModel() { identity_ = {"streak-aware", "1.0.0", FEATURE_VERSION_V2, "targets-v1"}; }
  const ModelIdentity& identity() const override { return identity_; }
  PredictionOutput predict(const FeatureVector& features, ThresholdTarget target,
                           const Regime* regime) const override {
    const double base = features.values.count("ewma_hit_13")
        ? features.values.at("ewma_hit_13") : EMPIRICAL_BASE_1_30;
    std::optional<double> streak;
    if (features.values.count("short_hit_13")) streak = features.values.at("short_hit_13");
    double p = streakAwareProbability(base, streak);
    if (target >= 2.0) p *= 0.55;
    return detail::makeSimpleOut(identity_, clamp01(p), target, regime,
                                 features.meta.dataQualityScore, {"streak-aware-model"});
  }
private:
  ModelIdentity identity_;
};

class RegimeAdjustedModel : public PredictiveModel {
public:
  explicit RegimeAdjustedModel(bool allowBoost = false) : allowBoost_(allowBoost) {
    identity_ = {"regime-adjusted", "1.0.0", FEATURE_VERSION_V2, "targets-v1"};
  }
  const ModelIdentity& identity() const override { return identity_; }
  PredictionOutput predict(const FeatureVector& features, ThresholdTarget target,
                           const Regime* regime) const override {
    const double base = features.values.count("ewma_hit_13")
        ? features.values.at("ewma_hit_13") : EMPIRICAL_BASE_1_30;
    const std::string rid = regime ? regime->id : "neutral";
    double p = regimeAdjustedProbability(base, rid, 0.05, allowBoost_);
    if (target >= 2.0) p *= 0.55;
    return detail::makeSimpleOut(identity_, clamp01(p), target, regime,
        features.meta.dataQualityScore,
        {"regime-adjusted-model", allowBoost_ ? "boost enabled" : "boost gated off (deliberate deviation)"});
  }
private:
  ModelIdentity identity_;
  bool allowBoost_ = false;
};

class MarkovModel : public PredictiveModel {
public:
  MarkovModel() { identity_ = {"markov", "1.0.0", FEATURE_VERSION_V2, "targets-v1"}; }
  const ModelIdentity& identity() const override { return identity_; }
  // Uses feature bag markov_p_up when engine not passed
  PredictionOutput predict(const FeatureVector& features, ThresholdTarget target,
                           const Regime* regime) const override {
    double p = features.values.count("markov_p_up") ? features.values.at("markov_p_up")
                                                    : EMPIRICAL_BASE_1_30;
    if (target >= 2.0) p *= 0.55;
    return detail::makeSimpleOut(identity_, clamp01(p), target, regime,
                                 features.meta.dataQualityScore, {"markov-model"});
  }
  CandidateEstimate score(const IncrementalStateEngine& eng) const {
    return markovChainModel(eng);
  }
private:
  ModelIdentity identity_;
};

class SpectralModel : public PredictiveModel {
public:
  SpectralModel() { identity_ = {"spectral", "1.0.0", FEATURE_VERSION_V2, "targets-v1"}; }
  const ModelIdentity& identity() const override { return identity_; }
  PredictionOutput predict(const FeatureVector& features, ThresholdTarget target,
                           const Regime* regime) const override {
    double p = features.values.count("ewma_hit_13") ? features.values.at("ewma_hit_13")
                                                    : EMPIRICAL_BASE_1_30;
    if (target >= 2.0) p *= 0.55;
    return detail::makeSimpleOut(identity_, clamp01(p), target, regime,
                                 features.meta.dataQualityScore, {"spectral-model"});
  }
  CandidateEstimate score(const IncrementalStateEngine& eng) const {
    return spectralModel(eng);
  }
private:
  ModelIdentity identity_;
};

class EntropyModel : public PredictiveModel {
public:
  EntropyModel() { identity_ = {"entropy", "1.0.0", FEATURE_VERSION_V2, "targets-v1"}; }
  const ModelIdentity& identity() const override { return identity_; }
  PredictionOutput predict(const FeatureVector& features, ThresholdTarget target,
                           const Regime* regime) const override {
    double p = features.values.count("short_hit_13") ? features.values.at("short_hit_13")
                                                     : EMPIRICAL_BASE_1_30;
    if (target >= 2.0) p *= 0.55;
    return detail::makeSimpleOut(identity_, clamp01(p), target, regime,
                                 features.meta.dataQualityScore, {"entropy-model"});
  }
  CandidateEstimate score(const IncrementalStateEngine& eng) const {
    return entropyModel(eng);
  }
private:
  ModelIdentity identity_;
};

class AutocorrelationModel : public PredictiveModel {
public:
  AutocorrelationModel() { identity_ = {"autocorrelation", "1.0.0", FEATURE_VERSION_V2, "targets-v1"}; }
  const ModelIdentity& identity() const override { return identity_; }
  PredictionOutput predict(const FeatureVector& features, ThresholdTarget target,
                           const Regime* regime) const override {
    double p = features.values.count("ewma_hit_13") ? features.values.at("ewma_hit_13")
                                                    : EMPIRICAL_BASE_1_30;
    if (target >= 2.0) p *= 0.55;
    return detail::makeSimpleOut(identity_, clamp01(p), target, regime,
                                 features.meta.dataQualityScore, {"autocorrelation-model"});
  }
  CandidateEstimate score(const IncrementalStateEngine& eng) const {
    return autocorrelationModel(eng);
  }
private:
  ModelIdentity identity_;
};

} // namespace pe
