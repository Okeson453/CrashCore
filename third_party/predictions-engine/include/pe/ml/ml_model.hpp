#pragma once

#include "pe/ml/ml_types.hpp"

namespace pe::ml {

class MLModel {
public:
  virtual ~MLModel() = default;
  virtual MLInferenceResult predict(const MLFeatureVector& features) const noexcept = 0;
  virtual std::size_t featureCount() const noexcept = 0;
  virtual const MLModelMetadata& metadata() const noexcept = 0;
};

} // namespace pe::ml
