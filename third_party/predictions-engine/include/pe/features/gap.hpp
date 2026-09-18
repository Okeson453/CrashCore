#pragma once

#include "pe/state/incremental_state_engine.hpp"

#include <cmath>
#include <string>
#include <unordered_map>

namespace pe {

inline std::unordered_map<std::string, double> computeGapFeatures(
    const IncrementalStateEngine& engine) {
  const double gap = engine.lastGapS();
  const double lag1 = engine.lagN(1);
  return {
    {"gap_s", gap},
    {"log_lag_1", lag1 > 0.0 ? std::log(lag1) : 0.0},
    {"gap_count", static_cast<double>(engine.gapCount())},
  };
}

} // namespace pe
