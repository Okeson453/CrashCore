#pragma once

#include "pe/ml/ml_calibrator.hpp"
#include "pe/ml/ml_fusion.hpp"
#include "pe/ml/ml_inference.hpp"
#include "pe/ml/ml_preprocessor.hpp"
#include "pe/ml/ml_runtime.hpp"
#include "pe/ml/ml_types.hpp"
#include "pe/types.hpp"

#include <array>
#include <optional>

namespace pe::ml {

class MLPredictor {
public:
  explicit MLPredictor(MLRuntime* runtime = nullptr) : runtime_(runtime) {}

  void setRuntime(MLRuntime* runtime) noexcept { runtime_ = runtime; }

  /**
   * Full path: FeatureVector → preprocess → normalize → infer → calibrate.
   * Returns nullopt if ML disabled / not LIVE / invalid features.
   * Never fabricates a probability.
   */
  std::optional<MLInferenceResult> predict(const pe::FeatureVector& features,
                                           const pe::Regime* regime = nullptr) const noexcept;

private:
  MLRuntime* runtime_ = nullptr;
  // Per-call stack buffers — no heap on hot path
  mutable std::array<double, MAX_ML_FEATURES> buf_{};
  mutable std::array<double, MAX_ML_FEATURES> norm_buf_{};
};

} // namespace pe::ml
