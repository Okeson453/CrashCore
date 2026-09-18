#pragma once

#include "pe/ml/ml_calibrator.hpp"
#include "pe/ml/ml_health.hpp"
#include "pe/ml/ml_inference.hpp"
#include "pe/ml/ml_metrics.hpp"
#include "pe/ml/ml_model.hpp"
#include "pe/ml/ml_model_config.hpp"
#include "pe/ml/ml_model_loader.hpp"
#include "pe/ml/ml_normalizer.hpp"

#include <atomic>
#include <memory>
#include <mutex>
#include <string>

namespace pe::ml {

enum class MLRuntimeState {
  Loading,
  Validating,
  Warming,
  Live,
  Degraded,
  Failed
};

inline const char* toString(MLRuntimeState s) {
  switch (s) {
    case MLRuntimeState::Loading:    return "LOADING";
    case MLRuntimeState::Validating: return "VALIDATING";
    case MLRuntimeState::Warming:    return "WARMING";
    case MLRuntimeState::Live:       return "LIVE";
    case MLRuntimeState::Degraded:   return "DEGRADED";
    case MLRuntimeState::Failed:     return "FAILED";
  }
  return "UNKNOWN";
}

class MLRuntime {
public:
  bool loadFromConfig(const MLModelConfig& cfg);
  /** Install in-memory default model (tests / SHADOW without artifacts). */
  bool loadDefault();

  MLRuntimeState state() const noexcept {
    return state_.load(std::memory_order_acquire);
  }

  const MLModel* activeModel() const noexcept {
    if (state() != MLRuntimeState::Live) return nullptr;
    return model_.get();
  }

  const MLNormalizer* normalizer() const noexcept {
    return has_normalizer_ ? &normalizer_ : nullptr;
  }
  const MLCalibrator* calibrator() const noexcept {
    return has_calibrator_ ? &calibrator_ : nullptr;
  }

  MLHealth& health() noexcept { return health_; }
  const MLHealth& health() const noexcept { return health_; }
  MLMetrics& metrics() noexcept { return metrics_; }
  const MLMetrics& metrics() const noexcept { return metrics_; }

  const MLModelConfig& config() const noexcept { return cfg_; }

  void markDegraded() noexcept {
    if (state() == MLRuntimeState::Live)
      state_.store(MLRuntimeState::Degraded, std::memory_order_release);
  }

  void checkHealth() noexcept {
    if (health_.shouldDegrade()) markDegraded();
  }

private:
  std::atomic<MLRuntimeState> state_{MLRuntimeState::Failed};
  MLModelConfig cfg_;
  std::unique_ptr<MLModel> model_;
  MLNormalizer normalizer_;
  MLCalibrator calibrator_;
  bool has_normalizer_ = false;
  bool has_calibrator_ = false;
  MLHealth health_;
  MLMetrics metrics_;
};

} // namespace pe::ml
