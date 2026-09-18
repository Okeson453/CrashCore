#pragma once

#include "pe/ml/ml_fusion.hpp"
#include "pe/ml/ml_predictor.hpp"
#include "pe/ml/ml_runtime.hpp"
#include "pe/models/predictive_model.hpp"
#include "pe/models/baseline_model.hpp"
#include "pe/features/feature_meta.hpp"
#include "pe/util/time.hpp"
#include "pe/util/uuid.hpp"

#include <chrono>
#include <cmath>
#include <memory>
#include <string>

namespace pe {

/**
 * Adapter: pe::ml path → PredictiveModel.
 * Candidate only — never the registry default.
 * Fusion blends statistical base with ML when available; otherwise statistical passthrough.
 */
class MLPredictiveModel final : public PredictiveModel {
public:
  explicit MLPredictiveModel(pe::ml::MLRuntime* runtime = nullptr,
                             double ml_weight = 0.0)
    : runtime_(runtime),
      predictor_(runtime),
      fusion_(pe::ml::MLFusion::fixedWeight(ml_weight)),
      ml_weight_(ml_weight) {
    identity_ = {"ml-predictive", "1.0.0", FEATURE_VERSION_V2, "targets-v1"};
  }

  void setRuntime(pe::ml::MLRuntime* runtime) noexcept {
    runtime_ = runtime;
    predictor_.setRuntime(runtime);
  }

  void setMlWeight(double w) noexcept {
    ml_weight_ = w;
    fusion_ = pe::ml::MLFusion::fixedWeight(w);
  }

  void setStatisticalBase(double probability, double confidence) noexcept {
    stat_p_ = probability;
    stat_conf_ = confidence;
  }

  const ModelIdentity& identity() const override { return identity_; }

  PredictionOutput predict(const FeatureVector& features,
                           ThresholdTarget target,
                           const Regime* regime) const override {
    using Clock = std::chrono::steady_clock;
    const auto t0 = Clock::now();

    // Statistical base (from last setStatisticalBase or feature ewma)
    double base_p = stat_p_;
    if (base_p <= 0.0) {
      auto it = features.values.find("ewma_hit_13");
      base_p = it != features.values.end() ? it->second : EMPIRICAL_BASE_1_30;
    }
    if (target >= 2.0) base_p *= 0.55;
    if (target >= 5.0) base_p *= 0.4;
    base_p = std::min(0.99, std::max(0.01, base_p));

    double final_p = base_p;
    double conf = stat_conf_ > 0.0 ? stat_conf_ : 0.5;
    std::vector<std::string> reasoning{"ml-predictive-model"};

    auto ml = predictor_.predict(features, regime);
    if (ml && ml->valid && runtime_ && runtime_->config().fusion_enabled && ml_weight_ > 0.0) {
      pe::ml::FusionInput fin;
      fin.statistical_probability = base_p;
      fin.ml_probability = ml->probability;
      fin.statistical_confidence = conf;
      fin.ml_confidence = 0.55;
      fin.regime_confidence = regime ? regime->confidence : 0.5;
      auto t1 = Clock::now();
      auto fused = fusion_.fuse(fin);
      if (runtime_) {
        runtime_->metrics().recordStage(pe::ml::MLMetrics::kFuse,
            std::chrono::duration<double, std::micro>(Clock::now() - t1).count());
      }
      final_p = fused.probability;
      conf = fused.confidence;
      reasoning.push_back("fusion ml_weight=" + std::to_string(ml_weight_));
      reasoning.push_back("ml_p=" + std::to_string(ml->probability));
    } else if (ml && ml->valid && ml_weight_ <= 0.0) {
      // SHADOW: ML ran but fusion ignores it
      reasoning.push_back("shadow ml_p=" + std::to_string(ml->probability));
    } else {
      reasoning.push_back("ml unavailable — statistical only");
    }

    PredictionOutput out;
    out.predictionId = util::randomUUID();
    out.model = identity_;
    out.target = target;
    out.score = final_p;
    out.probability = final_p;
    out.confidence = conf;
    if (regime) out.regime = *regime;
    out.dataQuality = features.meta.dataQualityScore;
    out.featureSummary = {
      {"stat_p", base_p},
      {"final_p", final_p},
      {"ml_weight", ml_weight_},
    };
    out.reasoning = std::move(reasoning);
    out.timestamp = util::nowIso8601();
    out.expiresAt = util::expiresAtIso(30);
    (void)t0;
    return out;
  }

private:
  ModelIdentity identity_;
  pe::ml::MLRuntime* runtime_ = nullptr;
  pe::ml::MLPredictor predictor_;
  pe::ml::MLFusion fusion_;
  double ml_weight_ = 0.0;
  mutable double stat_p_ = 0.0;
  mutable double stat_conf_ = 0.0;
};

} // namespace pe
