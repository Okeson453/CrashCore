#pragma once

#include "pe/state/incremental_state_engine.hpp"

#include <cmath>
#include <string>
#include <unordered_map>

namespace pe {

inline std::unordered_map<std::string, double> computeLagFeatures(
    const IncrementalStateEngine& engine) {
  std::unordered_map<std::string, double> out;
  const double l1 = engine.lagN(1);
  const double l2 = engine.lagN(2);
  const double l3 = engine.lagN(3);
  const double l5 = engine.lagN(5);
  out["lag_1"] = l1;
  out["lag_2"] = l2;
  out["lag_3"] = l3;
  out["lag_5"] = l5;
  out["lag_diff_1"] = l1 - l2;
  out["lag_ratio_1"] = (l2 > 1e-9) ? l1 / l2 : 0.0;
  return out;
}

} // namespace pe
