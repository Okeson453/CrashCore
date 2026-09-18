#include "pe/ml/ml_runtime.hpp"
#include "pe/ml/ml_feature_vector.hpp"

#include <vector>

namespace pe::ml {

bool MLRuntime::loadFromConfig(const MLModelConfig& cfg) {
  cfg_ = cfg;
  state_.store(MLRuntimeState::Loading, std::memory_order_release);

  if (!cfg.enabled) {
    state_.store(MLRuntimeState::Failed, std::memory_order_release);
    return false;
  }

  state_.store(MLRuntimeState::Validating, std::memory_order_release);

  std::optional<LoadedModel> loaded;
  if (!cfg.model_path.empty()) {
    loaded = loadModelFromBinary(cfg.model_path, cfg.checksum_path, cfg.schema_path, cfg.metadata_path);
    // Fail closed when an explicit path was given but load failed
    if (!loaded) {
      state_.store(MLRuntimeState::Failed, std::memory_order_release);
      return false;
    }
  } else {
    // No artifact path → in-memory default (SHADOW / unit tests)
    loaded = makeDefaultLogisticModel(
        cfg.expected_features > 0 ? cfg.expected_features : mlFeatureOrder().size());
  }

  state_.store(MLRuntimeState::Warming, std::memory_order_release);
  model_ = std::move(loaded->model);
  normalizer_ = std::move(loaded->normalizer);
  calibrator_ = std::move(loaded->calibrator);
  has_normalizer_ = loaded->has_normalizer;
  has_calibrator_ = loaded->has_calibrator;

  std::vector<double> zeros(model_->featureCount(), 0.0);
  MLFeatureVector fv{zeros.data(), zeros.size()};
  MLInference infer(model_.get());
  if (!infer.run(fv).valid) {
    state_.store(MLRuntimeState::Failed, std::memory_order_release);
    model_.reset();
    return false;
  }

  health_.reset();
  metrics_.reset();
  state_.store(MLRuntimeState::Live, std::memory_order_release);
  return true;
}

bool MLRuntime::loadDefault() {
  MLModelConfig cfg;
  cfg.enabled = true;
  cfg.expected_features = mlFeatureOrder().size();
  cfg.model_path.clear();
  cfg.checksum_path.clear();
  return loadFromConfig(cfg);
}

} // namespace pe::ml
