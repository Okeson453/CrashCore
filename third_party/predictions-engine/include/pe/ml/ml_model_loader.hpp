#pragma once

#include "pe/ml/ml_calibrator.hpp"
#include "pe/ml/ml_model.hpp"
#include "pe/ml/ml_normalizer.hpp"
#include "pe/ml/ml_types.hpp"

#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace pe::ml {

struct LoadedModel {
  std::unique_ptr<MLModel> model;
  MLModelMetadata metadata;
  MLNormalizer normalizer;
  MLCalibrator calibrator;
  bool has_normalizer = false;
  bool has_calibrator = false;
};

/**
 * Load PEML v1 binary. Fails closed on:
 *  - bad magic/version
 *  - feature_count != ML_FEATURE_ORDER size
 *  - checksum mismatch (when checksumPath non-empty)
 *  - failed warm-up inference
 */
std::optional<LoadedModel> loadModelFromBinary(const std::string& modelPath,
                                               const std::string& checksumPath = {},
                                               const std::string& schemaPath = {},
                                               const std::string& metadataPath = {});

LoadedModel makeDefaultLogisticModel(std::size_t feature_count = 0);

} // namespace pe::ml
