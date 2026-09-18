#pragma once

#include "pe/state/incremental_state_engine.hpp"

#include <string>
#include <unordered_map>

namespace pe {

inline std::unordered_map<std::string, double> computeRunFeatures(
    const IncrementalStateEngine& engine) {
  const auto& r = engine.runs();
  return {
    {"run_below_1_30", static_cast<double>(r.below13)},
    {"run_above_1_30", static_cast<double>(r.above13)},
    {"run_below_1_50", static_cast<double>(r.below15)},
    {"run_below_2_00", static_cast<double>(r.below20)},
    {"run_above_2_00", static_cast<double>(r.above20)},
    {"max_run_below_1_30", static_cast<double>(r.maxBelow13)},
    {"max_run_above_1_30", static_cast<double>(r.maxAbove13)},
    {"max_run_below_2_00", static_cast<double>(r.maxBelow20)},
  };
}

} // namespace pe
