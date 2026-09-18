#pragma once

/**
 * Gap-conditional model (Model A/B candidate). Full port of gap-conditional-model.ts.
 * Self-contained — no external imports.
 */

#include "pe/features/feature_meta.hpp"
#include "pe/models/baseline_model.hpp"
#include "pe/models/candidate_models.hpp"
#include "pe/models/predictive_model.hpp"
#include "pe/state/incremental_state_engine.hpp"
#include "pe/util/time.hpp"
#include "pe/util/uuid.hpp"

#include <algorithm>
#include <cmath>
#include <string>
#include <vector>

namespace pe {

/** Report §5 Model A coefficients (log_m_lag1-only logistic). */
struct ModelACoeffs { double intercept = 1.145; double logLag1 = -0.023; };
/** Report §5 Model B coefficients (gap_z + log_lag1_z logistic). */
struct ModelBCoeffs { double intercept = 1.203; double gapZ = 1.491; double logLag1Z = -0.309; };

struct GapZParams {
  double gapMeanS = 0.0;
  double gapStdS = 1.0;
  double logLag1Mean = 0.0;
  double logLag1Std = 1.0;
};

inline constexpr GapZParams NEUTRAL_Z{};

inline double sigmoid(double x) { return 1.0 / (1.0 + std::exp(-x)); }

class GapConditionalModel : public PredictiveModel {
public:
  GapConditionalModel() {
    identity_ = {"gap-conditional", "1.0.0", FEATURE_VERSION_V2, "tv-1.0.0"};
  }

  const ModelIdentity& identity() const override { return identity_; }

  void setGapSignalActive(bool active) { gapSignalActive_ = active; }
  bool isGapSignalActive() const { return gapSignalActive_; }

  void setStandardization(const GapZParams& z) {
    z_ = z;
    if (z_.gapStdS <= 0.0) z_.gapStdS = 1.0;
    if (z_.logLag1Std <= 0.0) z_.logLag1Std = 1.0;
  }
  const GapZParams& getStandardization() const { return z_; }

  void fit(const Dataset& trainingData) override {
    std::vector<double> gaps, logLags;
    for (const auto& row : trainingData.rows) {
      if (row.features.values.count("gap_s"))
        gaps.push_back(row.features.values.at("gap_s"));
      if (row.features.values.count("log_lag_1"))
        logLags.push_back(row.features.values.at("log_lag_1"));
    }
    auto meanStd = [](const std::vector<double>& xs) -> std::pair<double, double> {
      if (xs.empty()) return {0.0, 1.0};
      double m = 0.0;
      for (double x : xs) m += x;
      m /= static_cast<double>(xs.size());
      double v = 0.0;
      for (double x : xs) { const double d = x - m; v += d * d; }
      v = xs.size() > 1 ? v / static_cast<double>(xs.size() - 1) : 1.0;
      return {m, std::sqrt(v) > 0.0 ? std::sqrt(v) : 1.0};
    };
    const auto [gm, gs] = meanStd(gaps);
    const auto [lm, ls] = meanStd(logLags);
    z_ = {gm, gs, lm, ls};
  }

  PredictionOutput predict(const FeatureVector& features, ThresholdTarget target,
                           const Regime* regime) const override {
    auto get = [&](const std::string& k, double d = 0.0) {
      auto it = features.values.find(k);
      return it != features.values.end() ? it->second : d;
    };
    const double gapS = get("gap_s", 0.0);
    const double logLag1 = get("log_lag_1", 0.0);

    double probability = 0.0;
    std::vector<std::string> reasoning;

    if (gapSignalActive_) {
      // Model B: gap_z + log_lag1_z
      const double gapZ = (gapS - z_.gapMeanS) / z_.gapStdS;
      const double logZ = (logLag1 - z_.logLag1Mean) / z_.logLag1Std;
      const double logit = MODEL_B.intercept + MODEL_B.gapZ * gapZ + MODEL_B.logLag1Z * logZ;
      probability = clamp01(sigmoid(logit));
      reasoning.push_back("MODEL_B: gap regime active (gap_z + log_lag1_z logistic)");
    } else {
      // Model A: log_lag1 only (default / inactive)
      const double logit = MODEL_A.intercept + MODEL_A.logLag1 * logLag1;
      probability = clamp01(sigmoid(logit));
      reasoning.push_back("MODEL_A: gap regime inactive (log_lag1-only logistic)");
    }

    if (target != 1.3) {
      probability = clamp01(1.0 / target);
      reasoning.push_back("coefficient set is 1.30-specific — base-rate fallback");
    }

    PredictionOutput out;
    out.predictionId = util::randomUUID();
    out.model = identity_;
    out.target = target;
    out.score = probability;
    out.probability = probability;
    out.confidence = 0.3; // candidate-grade
    if (regime) out.regime = *regime;
    out.dataQuality = features.meta.dataQualityScore;
    out.featureSummary = {
      {"gap_s", gapS},
      {"log_lag_1", logLag1},
      {"gap_regime_active", gapSignalActive_ ? 1.0 : 0.0},
      {"sample_size", static_cast<double>(features.meta.sampleSize)},
    };
    out.reasoning = std::move(reasoning);
    out.timestamp = util::nowIso8601();
    out.expiresAt = util::expiresAtIso(60);
    return out;
  }

private:
  ModelIdentity identity_;
  bool gapSignalActive_ = false;
  GapZParams z_ = NEUTRAL_Z;
  static constexpr ModelACoeffs MODEL_A{};
  static constexpr ModelBCoeffs MODEL_B{};
};

/** Candidate scorer from incremental engine gap state. */
inline CandidateEstimate scoreGapConditional(const IncrementalStateEngine& engine) {
  FeatureVector fv;
  fv.roundId = "pipeline";
  fv.timestamp = util::nowIso8601();
  fv.featureVersion = FEATURE_VERSION_V2;
  fv.values["gap_s"] = engine.lastGapS();
  const double lag1 = engine.lagN(1);
  fv.values["log_lag_1"] = lag1 >= 1.0 ? std::log(lag1) : 0.0;
  fv.meta = FeatureVector::Meta{0, 1.0, 0, std::nullopt};
  static GapConditionalModel model; // shared for scoring
  const auto out = model.predict(fv, 1.3, nullptr);
  return {"GapConditionalModel", out.probability};
}

inline std::vector<CandidateEstimate> scoreCandidates(const IncrementalStateEngine& engine) {
  return {
    autocorrelationModel(engine),
    markovChainModel(engine),
    spectralModel(engine),
    entropyModel(engine),
    scoreGapConditional(engine),
  };
}

} // namespace pe
