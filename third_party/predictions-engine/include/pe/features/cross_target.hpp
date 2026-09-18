#pragma once

#include "pe/state/incremental_state_engine.hpp"

#include <string>
#include <unordered_map>

namespace pe {

inline std::unordered_map<std::string, double> computeCrossTargetFeatures(
    const IncrementalStateEngine& engine) {
  const double h13 = engine.hitRate(1.3);
  const double h20 = engine.hitRate(2.0);
  const double h50 = engine.hitRate(5.0);
  return {
    {"hit_13", h13},
    {"hit_20", h20},
    {"hit_50", h50},
    {"hit_ratio_20_13", h13 > 1e-6 ? h20 / h13 : 0.0},
    {"hit_ratio_50_13", h13 > 1e-6 ? h50 / h13 : 0.0},
    {"hit_spread_20_13", h13 - h20},
  };
}

} // namespace pe
