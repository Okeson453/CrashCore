#pragma once

#include "pe/ml/ml_feature_vector.hpp"
#include "pe/ml/ml_types.hpp"
#include "pe/types.hpp"

#include <cstddef>

namespace pe::ml {

/**
 * Reorders pe::FeatureVector map into ML_FEATURE_ORDER flat buffer.
 * Writes into caller-owned out[0..count); no allocation.
 * Returns false on NaN/Inf or size mismatch.
 */
bool preprocess(const pe::FeatureVector& features,
                double* out,
                std::size_t capacity,
                std::size_t* written) noexcept;

} // namespace pe::ml
