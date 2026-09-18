#pragma once

/**
 * PredictionEngine orchestrator — composes state → features → regime → model → signal.
 * Ported from prediction-engine.ts
 */

#include "pe/features/feature_engine_v2.hpp"
#include "pe/models/model_registry.hpp"
#include "pe/regimes/regime_detector.hpp"
#include "pe/signals/signal.hpp"
#include "pe/signals/validate.hpp"
#include "pe/state/incremental_state_engine.hpp"
#include "pe/types.hpp"
#include "pe/util/logger.hpp"
#include "pe/util/time.hpp"
#include "pe/util/uuid.hpp"
#include "pe/util/latency_budget.hpp"
#ifdef PE_ENABLE_ML
#include "pe/ml/ml_predictor.hpp"
#include "pe/ml/ml_fusion.hpp"
#include "pe/models/ml_predictive_model.hpp"
#endif

#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace pe {

struct PredictRequest {
  std::string targetRoundId;
  ThresholdTarget target = 1.3;
  std::optional<std::string> modelName;
  std::optional<std::string> modelVersion;
  /** Optional recent crash points for regime detector (if not using engine lag). */
  std::vector<double> recentCrashPoints;
};

struct PredictResult {
  PredictionSignal signal;
  PredictionOutput output;
  FeatureVector features;
  Regime regime;
  EngineLifecycleState lifecycle = EngineLifecycleState::COLD;
  /** Wall time for this predict() call (microseconds). */
  int64_t latencyUs = 0;
  /** True if latencyUs <= LATENCY_BUDGET_US (5 ms hard ceiling). */
  bool withinBudget = true;
};

class PredictionEngine {
public:
  PredictionEngine()
    : state_(std::make_unique<IncrementalStateEngine>()),
      features_(std::make_unique<FeatureEngineV2>()),
      regimes_(std::make_unique<RegimeDetector>()),
      models_(std::make_unique<ModelRegistry>()) {}

  IncrementalStateEngine& state() { return *state_; }
  const IncrementalStateEngine& state() const { return *state_; }
  ModelRegistry& models() { return *models_; }

  /** Feed a new crash observation (O(1)). */
  IncrementalEngineSnapshot observe(double crashPoint, std::optional<int64_t> beganAtMs = std::nullopt) {
    if (beganAtMs) state_->recordBeganAt(*beganAtMs);
    return state_->update(crashPoint);
  }

  PredictResult predict(const PredictRequest& req) {
    LatencyTimer timer;
    PredictResult result;
    result.lifecycle = state_->getLifecycleState();

    // 1. Feature generation
    FeatureVector fv;
    try {
      fv = features_->compute(*state_, req.targetRoundId);
    } catch (const std::exception& e) {
      throw PipelineStageError(PipelineStage::FeatureGeneration, e.what());
    }

    // 2. Regime detection
    Regime regime;
    try {
      std::vector<double> cps = req.recentCrashPoints;
      if (cps.empty()) cps = state_->getLagArray();
      regime = regimes_->detect(cps);
    } catch (const std::exception& e) {
      throw PipelineStageError(PipelineStage::RegimeDetection, e.what());
    }

    // 3. Model resolution
    PredictiveModel* model = nullptr;
    try {
      if (req.modelName) {
        model = models_->get(*req.modelName, req.modelVersion.value_or(""));
        if (!model)
          throw std::runtime_error("model not found: " + *req.modelName);
      } else {
        model = &models_->getDefault();
      }
    } catch (const std::exception& e) {
      throw PipelineStageError(PipelineStage::ModelResolution, e.what());
    }

    // 4. Model prediction
    PredictionOutput out;
    try {
      out = model->predict(fv, req.target, &regime);
    } catch (const std::exception& e) {
      throw PipelineStageError(PipelineStage::ModelPrediction, e.what());
    }

#ifdef PE_ENABLE_ML
    // 4b/4c. Optional ML inference + fusion (no-op when disabled / not LIVE / already ML model)
    // Failures here do not fail the prediction — fall through to statistical output.
    try {
      auto* rt = models_->mlRuntime();
      const bool already_ml = (model->identity().name == "ml-predictive");
      if (rt && rt->state() == pe::ml::MLRuntimeState::Live
          && rt->config().enabled && !already_ml
          && (rt->config().fusion_enabled || rt->config().ml_weight > 0.0)) {
        pe::ml::MLPredictor pred(rt);
        auto ml = pred.predict(fv, &regime);
        if (ml && ml->valid && rt->config().ml_weight > 0.0 && rt->config().fusion_enabled) {
          pe::ml::MLFusion fusion = pe::ml::MLFusion::fixedWeight(rt->config().ml_weight);
          pe::ml::FusionInput fin;
          fin.statistical_probability = out.probability;
          fin.ml_probability = ml->probability;
          fin.statistical_confidence = out.confidence;
          fin.ml_confidence = 0.55;
          fin.regime_confidence = regime.confidence;
          auto fused = fusion.fuse(fin);
          out.probability = fused.probability;
          out.score = fused.probability;
          out.confidence = fused.confidence;
          out.reasoning.push_back("engine_fusion ml_weight=" + std::to_string(rt->config().ml_weight));
          out.featureSummary["ml_p"] = ml->probability;
        } else if (ml && ml->valid) {
          // SHADOW: record only
          out.featureSummary["ml_p_shadow"] = ml->probability;
        }
      }
    } catch (const std::exception& e) {
      // swallow — ML stage failure must not fail the prediction
      (void)e;
    }
#endif

    // 5. Prediction output validation
    try {
      validatePredictionOutput(out);
    } catch (const PipelineStageError&) {
      throw;
    } catch (const std::exception& e) {
      throw PipelineStageError(PipelineStage::PredictionOutputValidation, e.what());
    }

    // 6. Signal conversion
    PredictionSignal signal;
    try {
      signal = toSignal(out, FeaturePath::V2_INCREMENTAL, req.targetRoundId);
    } catch (const std::exception& e) {
      throw PipelineStageError(PipelineStage::SignalConversion, e.what());
    }

    // 7. Signal validation
    try {
      validateSignal(signal);
    } catch (const PipelineStageError&) {
      throw;
    } catch (const std::exception& e) {
      throw PipelineStageError(PipelineStage::SignalValidation, e.what());
    }

    result.signal = std::move(signal);
    result.output = std::move(out);
    result.features = std::move(fv);
    result.regime = std::move(regime);
    result.latencyUs = timer.elapsedUs();
    result.withinBudget = timer.withinBudget();
    return result;
  }

private:
  std::unique_ptr<IncrementalStateEngine> state_;
  std::unique_ptr<FeatureEngineV2> features_;
  std::unique_ptr<RegimeDetector> regimes_;
  std::unique_ptr<ModelRegistry> models_;
};

} // namespace pe
