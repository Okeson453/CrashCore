#pragma once

#include "pe/features/feature_meta.hpp"
#include "pe/types.hpp"

#include <stdexcept>
#include <string>

namespace pe {

inline void assertFeatureVersionMatch(const std::optional<std::string>& stored) {
  if (!stored || stored->empty()) return;
  if (*stored != FEATURE_VERSION_V2) {
    throw std::runtime_error(
      std::string("Feature version mismatch: prediction=") + *stored +
      " engine=" + FEATURE_VERSION_V2);
  }
}

} // namespace pe
