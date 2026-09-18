#include "pe/ml/ml_inference.hpp"

#include <chrono>

namespace pe::ml {

MLInferenceResult MLInference::run(const MLFeatureVector& features) const noexcept {
  MLInferenceResult r;
  if (!model_ || !features.data || features.size == 0) return r;
  if (features.size != model_->featureCount()) return r;

  using Clock = std::chrono::steady_clock;
  const auto t0 = Clock::now();
  r = model_->predict(features);
  const auto t1 = Clock::now();
  r.inference_us = std::chrono::duration<double, std::micro>(t1 - t0).count();
  return r;
}

} // namespace pe::ml
