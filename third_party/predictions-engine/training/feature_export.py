"""Single source of truth for ML_FEATURE_ORDER — keep in sync with ml_feature_vector.hpp."""
ML_FEATURE_ORDER = [
    "sample_size", "quality_score", "mean_crash", "std_crash", "ewma_crash", "ewma_hit_13",
    "hit_1_30_20", "hit_1_30_50", "hit_1_30_100", "hit_1_30_200",
    "short_mean", "short_std", "short_hit_13",
    "since_13", "since_20", "since_50", "since_100",
    "lag_1", "lag_2", "lag_3", "lag_5", "lag_diff_1", "lag_ratio_1",
    "run_below_1_30", "run_above_1_30", "run_below_2_00", "run_above_2_00",
    "markov_p_up", "markov_p_stay_up", "markov_p_stay_down",
    "spectral_acf1", "spectral_acf2",
    "entropy_binary_13", "entropy_short_13", "entropy_ratio",
    "hour_sin", "hour_cos", "dow_sin", "dow_cos", "is_weekend",
    "hit_13", "hit_20", "hit_50", "hit_ratio_20_13", "hit_ratio_50_13",
    "gap_s", "log_lag_1", "gap_count",
]

def export_row(feature_map: dict) -> list[float]:
    return [float(feature_map.get(k, 0.0)) for k in ML_FEATURE_ORDER]
