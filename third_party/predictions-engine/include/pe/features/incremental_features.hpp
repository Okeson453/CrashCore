#pragma once

/**
 * Thin helpers mapping IncrementalStateEngine snapshot → common feature keys.
 * Port of incremental-features.ts. Self-contained.
 */

#include "pe/state/incremental_state_engine.hpp"

#include <cmath>
#include <string>
#include <unordered_map>

namespace pe {

inline std::unordered_map<std::string, double> computeIncrementalCoreFeatures(
    const IncrementalStateEngine& engine) {
  const auto windows = engine.hitRateWindows13();
  return {
    {"sample_size", static_cast<double>(engine.count())},
    {"mean_crash", engine.welford().mean},
    {"std_crash", engine.std()},
    {"ewma_crash", engine.ewma()},
    {"ewma_hit_13", engine.ewmaHit13()},
    {"hit_1_30_20", windows.w20},
    {"hit_1_30_50", windows.w50},
    {"hit_1_30_100", windows.w100},
    {"hit_1_30_200", windows.w200},
    {"short_mean", engine.shortMean()},
    {"short_std", std::sqrt(engine.shortVariance())},
    {"short_hit_13", engine.shortHitRate13()},
    {"since_13", static_cast<double>(engine.roundsSince(1.3))},
    {"since_20", static_cast<double>(engine.roundsSince(2.0))},
    {"since_50", static_cast<double>(engine.roundsSince(5.0))},
    {"since_100", static_cast<double>(engine.roundsSince(10.0))},
    {"run_below_1_30", static_cast<double>(engine.runs().below13)},
    {"run_above_1_30", static_cast<double>(engine.runs().above13)},
    {"markov_p_up", engine.markovPNextAbove13()},
  };
}

} // namespace pe
