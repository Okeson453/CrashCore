#pragma once

/**
 * Meta-logistic model — simple logistic combination of a few features.
 * Ported / simplified from meta-logistic-model.ts
 */

#include "pe/features/feature_meta.hpp"
#include "pe/models/baseline_model.hpp"
#include "pe/models/predictive_model.hpp"
#include "pe/util/time.hpp"
#include "pe/util/uuid.hpp"

#include <algorithm>
#include <cmath>
#include <string>
#include <vector>

namespace pe {

class MetaLogisticModel : public PredictiveModel {
public:
  MetaLogisticModel() {
    identity_ = {"meta-logistic", "1.0.0", FEATURE_VERSION_V2, "targets-v1"};
    // Default weights: intercept, ewma_hit_13, markov_p_up, short_hit_13, entropy_binary_13
    weights_ = {-0.2, 1.8, 0.6, 0.9, -0.3};
  }
  const ModelIdentity& identity() const override { return identity_; }

  PredictionOutput predict(const FeatureVector& features, ThresholdTarget target,
                           const Regime* regime) const override {
    auto get = [&](const std::string& k, double d = 0.0) {
      auto it = features.values.find(k);
      return it != features.values.end() ? it->second : d;
    };
    const std::vector<double> x = {
      1.0,
      get("ewma_hit_13", EMPIRICAL_BASE_1_30),
      get("markov_p_up", EMPIRICAL_BASE_1_30),
      get("short_hit_13", EMPIRICAL_BASE_1_30),
      get("entropy_binary_13", 1.0)
    };
    double logit = 0.0;
    for (size_t i = 0; i < weights_.size() && i < x.size(); ++i)
      logit += weights_[i] * x[i];
    double p = 1.0 / (1.0 + std::exp(-logit));
    if (target >= 2.0) p *= 0.55;
    if (target >= 5.0) p *= 0.4;
    p = std::clamp(p, 0.01, 0.99);

    PredictionOutput out;
    out.predictionId = util::randomUUID();
    out.model = identity_;
    out.target = target;
    out.score = p;
    out.probability = p;
    out.confidence = 0.6;
    if (regime) out.regime = *regime;
    out.dataQuality = features.meta.dataQualityScore;
    out.reasoning = {"meta-logistic-model"};
    out.timestamp = util::nowIso8601();
    out.expiresAt = util::expiresAtIso(30);
    return out;
  }
private:
  ModelIdentity identity_;
  std::vector<double> weights_;
};

} // namespace pe
