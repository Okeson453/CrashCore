#pragma once

#include "pe/models/baseline_model.hpp"
#include "pe/models/candidate_models.hpp"
#include "pe/models/gap_conditional_model.hpp"
#include "pe/models/meta_logistic_model.hpp"
#ifdef PE_ENABLE_ML
#include "pe/models/ml_predictive_model.hpp"
#include "pe/ml/ml_runtime.hpp"
#endif
#include "pe/models/predictive_model.hpp"
#include "pe/util/logger.hpp"

#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

namespace pe {

class ModelRegistry {
public:
  ModelRegistry() {
    auto baseline = std::make_unique<BaselineStatisticalModel>();
    defaultKey_ = keyOf(baseline->identity());
    registerModel(std::move(baseline));
    registerModel(std::make_unique<GapConditionalModel>());
    registerModel(std::make_unique<FrequencyModel>());
    registerModel(std::make_unique<StreakAwareModel>());
    registerModel(std::make_unique<RegimeAdjustedModel>(/*allowBoost=*/false));
    registerModel(std::make_unique<MetaLogisticModel>());
    registerModel(std::make_unique<MarkovModel>());
    registerModel(std::make_unique<SpectralModel>());
    registerModel(std::make_unique<EntropyModel>());
    registerModel(std::make_unique<AutocorrelationModel>());
#ifdef PE_ENABLE_ML
    // Candidate only — never default. SHADOW weight 0 until CANARY.
    ml_runtime_ = std::make_unique<pe::ml::MLRuntime>();
    ml_runtime_->loadDefault();
    auto ml = std::make_unique<MLPredictiveModel>(ml_runtime_.get(), /*ml_weight=*/0.0);
    registerModel(std::move(ml));
#endif
  }

#ifdef PE_ENABLE_ML
  pe::ml::MLRuntime* mlRuntime() noexcept { return ml_runtime_.get(); }
  const pe::ml::MLRuntime* mlRuntime() const noexcept { return ml_runtime_.get(); }
#endif

  void registerModel(std::unique_ptr<PredictiveModel> model) {
    const std::string key = keyOf(model->identity());
    models_[key] = std::move(model);
    util::info("Model registered: " + key);
  }

  PredictiveModel* get(const std::string& name, const std::string& version = "") {
    if (!version.empty()) {
      auto it = models_.find(name + "@" + version);
      return it != models_.end() ? it->second.get() : nullptr;
    }
    PredictiveModel* last = nullptr;
    for (auto& [k, m] : models_) {
      if (k.rfind(name + "@", 0) == 0) last = m.get();
    }
    return last;
  }

  PredictiveModel& getDefault() {
    auto it = models_.find(defaultKey_);
    if (it == models_.end()) throw std::runtime_error("No default model registered");
    return *it->second;
  }

  std::vector<ModelIdentity> list() const {
    std::vector<ModelIdentity> out;
    out.reserve(models_.size());
    for (const auto& [_, m] : models_) out.push_back(m->identity());
    return out;
  }

private:
  static std::string keyOf(const ModelIdentity& id) {
    return id.name + "@" + id.version;
  }

  std::unordered_map<std::string, std::unique_ptr<PredictiveModel>> models_;
  std::string defaultKey_;
#ifdef PE_ENABLE_ML
  std::unique_ptr<pe::ml::MLRuntime> ml_runtime_;
#endif
};

} // namespace pe
