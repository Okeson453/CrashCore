#pragma once

#include "pe/state/incremental_state_engine.hpp"

#include <cmath>
#include <string>
#include <unordered_map>

namespace pe {

inline double binaryEntropy(double p) {
  const double q = std::min(0.999, std::max(0.001, p));
  return -(q * std::log2(q) + (1.0 - q) * std::log2(1.0 - q));
}

inline std::unordered_map<std::string, double> computeEntropyFeatures(
    const IncrementalStateEngine& engine) {
  const double p = engine.hitRate(1.3);
  const double ps = engine.shortHitRate13();
  const double e = binaryEntropy(p);
  const double es = binaryEntropy(ps);
  return {
    {"entropy_binary_13", e},
    {"entropy_short_13", es},
    {"entropy_ratio", es / (e + 1e-9)},
  };
}

} // namespace pe
