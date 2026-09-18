#pragma once

#include <string>
#include <unordered_set>
#include <vector>

namespace pe {

struct FeatureMeta {
  std::string featureName;
  std::string featureVersion;
  std::string source;
  std::string updateCost; // "O(1)" | "O(w)" | "O(n)"
  std::vector<std::string> dependencies;
  double validityWindow = 0.0;
  std::string missingValuePolicy; // "zero" | "carry" | "skip"
};

/** fv-2.1.0: added the gap family (gap_s, log_lag_1) */
inline constexpr const char* FEATURE_VERSION_V2 = "fv-2.1.0";
inline constexpr const char* CURRENT_FEATURE_VERSION = FEATURE_VERSION_V2;

/** Known feature keys for schema validation (fixed to match actual family outputs). */
inline const std::unordered_set<std::string>& featureSchemaV2() {
  static const std::unordered_set<std::string> schema = {
    // quality / sample
    "quality_score", "sample_size",
    // lag family
    "lag_1", "lag_2", "lag_3", "lag_5", "lag_diff_1", "lag_ratio_1",
    // run family
    "run_below_1_30", "run_above_1_30", "run_below_1_50", "run_below_2_00",
    "run_above_2_00", "max_run_below_1_30", "max_run_above_1_30", "max_run_below_2_00",
    // markov
    "markov_p_up", "markov_p_stay_up", "markov_p_stay_down",
    // spectral
    "spectral_acf1", "spectral_acf2", "spectral_period_hint",
    // entropy
    "entropy_binary_13", "entropy_short_13", "entropy_ratio",
    // time
    "hour_sin", "hour_cos", "dow_sin", "dow_cos", "hour_utc", "is_weekend",
    // cross-target
    "hit_13", "hit_20", "hit_50", "hit_ratio_20_13", "hit_ratio_50_13", "hit_spread_20_13",
    // gap
    "gap_s", "log_lag_1", "gap_count",
    // window hit rates / stats from calculators / V2 assembly
    "hit_1_30_20", "hit_1_30_50", "hit_1_30_100", "hit_1_30_200",
    "mean_crash", "std_crash", "ewma_crash", "ewma_hit_13",
    "short_mean", "short_std", "short_hit_13",
    "since_13", "since_20", "since_50", "since_100",
  };
  return schema;
}

} // namespace pe
