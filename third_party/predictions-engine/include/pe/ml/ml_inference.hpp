#pragma once

#include "pe/ml/ml_model.hpp"
#include "pe/ml/ml_types.hpp"

namespace pe::ml {

class MLInference {
public:
  explicit MLInference(const MLModel* model = nullptr) : model_(model) {}

  void setModel(const MLModel* model) noexcept { model_ = model; }
  const MLModel* model() const noexcept { return model_; }

  MLInferenceResult run(const MLFeatureVector& features) const noexcept;

private:
  const MLModel* model_ = nullptr;
};

} // namespace pe::ml
