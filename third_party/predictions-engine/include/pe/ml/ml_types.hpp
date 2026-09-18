#pragma once

#include <cstddef>
#include <string>

namespace pe::ml {

struct MLFeatureVector {
  const double* data = nullptr;
  std::size_t size = 0;
};

struct MLInferenceResult {
  double probability = 0.0;
  double raw_score = 0.0;
  double inference_us = 0.0;
  bool valid = false;
};

struct MLModelMetadata {
  std::string model_id;
  std::string version;
  std::size_t feature_count = 0;
  std::string training_hash;
  std::string feature_schema_hash;
};

inline constexpr std::size_t MAX_ML_FEATURES = 128;

} // namespace pe::ml
