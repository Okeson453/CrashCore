#pragma once

#include <cstddef>
#include <string>

namespace pe::ml {

struct MLModelConfig {
  bool enabled = false;
  std::size_t expected_features = 0;
  double minimum_probability = 0.0;
  double maximum_probability = 1.0;
  bool calibration_enabled = true;
  bool fusion_enabled = true;
  double ml_weight = 0.0; // SHADOW default; CANARY/PRODUCTION override
  std::string model_path;
  std::string schema_path;
  std::string metadata_path;
  std::string checksum_path;
};

} // namespace pe::ml
