#include "pe/ml/ml_predictor.hpp"
#include "pe/ml/ml_metrics.hpp"

#include <chrono>
#include <cmath>

namespace pe::ml {

std::optional<MLInferenceResult> MLPredictor::predict(
    const pe::FeatureVector& features,
    const pe::Regime* /*regime*/) const noexcept {
  if (!runtime_) return std::nullopt;
  if (runtime_->state() != MLRuntimeState::Live) return std::nullopt;
  const MLModel* model = runtime_->activeModel();
  if (!model) return std::nullopt;

  using Clock = std::chrono::steady_clock;
  auto& metrics = runtime_->metrics();
  auto& health = runtime_->health();

  // --- preprocess ---
  auto t0 = Clock::now();
  std::size_t written = 0;
  if (!preprocess(features, buf_.data(), buf_.size(), &written)) {
    health.recordFeatureMismatch();
    runtime_->checkHealth();
    return std::nullopt;
  }
  if (written != model->featureCount()) {
    health.recordFeatureMismatch();
    runtime_->checkHealth();
    return std::nullopt;
  }
  auto t1 = Clock::now();
  metrics.recordStage(MLMetrics::kPrep,
      std::chrono::duration<double, std::micro>(t1 - t0).count());

  // --- normalize (optional) ---
  const double* feat_ptr = buf_.data();
  if (const auto* norm = runtime_->normalizer()) {
    norm->transform(buf_.data(), norm_buf_.data(), written);
    feat_ptr = norm_buf_.data();
  }

  // --- inference ---
  MLFeatureVector fv{feat_ptr, written};
  MLInference infer(model);
  auto result = infer.run(fv);
  metrics.recordStage(MLMetrics::kInfer, result.inference_us);

  if (!result.valid || !std::isfinite(result.probability)) {
    health.recordNaN();
    runtime_->checkHealth();
    return std::nullopt;
  }

  // --- calibrate ---
  auto t3 = Clock::now();
  if (runtime_->config().calibration_enabled) {
    if (const auto* cal = runtime_->calibrator()) {
      result.probability = cal->calibrate(result.probability);
    }
  }
  // clamp to config bounds
  result.probability = std::min(runtime_->config().maximum_probability,
                       std::max(runtime_->config().minimum_probability, result.probability));
  auto t4 = Clock::now();
  metrics.recordStage(MLMetrics::kCal,
      std::chrono::duration<double, std::micro>(t4 - t3).count());

  health.recordSuccess(result.inference_us);
  return result;
}

} // namespace pe::ml
