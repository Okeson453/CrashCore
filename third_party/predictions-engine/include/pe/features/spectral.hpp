#pragma once

#include "pe/state/incremental_state_engine.hpp"

#include <cmath>
#include <string>
#include <unordered_map>

namespace pe {

/**
 * Lag-1 / lag-2 autocorrelation + crude period hint.
 * Latency-bounded: uses at most SPECTRAL_CAP newest points (O(1) vs lag ring size).
 */
inline constexpr int SPECTRAL_CAP = 64;

inline std::unordered_map<std::string, double> computeSpectralFeatures(
    const IncrementalStateEngine& engine) {
  std::unordered_map<std::string, double> out;
  const int nFull = engine.lagLen();
  const int n = std::min(nFull, SPECTRAL_CAP);
  if (n < 4) {
    out["spectral_acf1"] = 0.0;
    out["spectral_acf2"] = 0.0;
    out["spectral_period_hint"] = 0.0;
    return out;
  }

  // Newest n points: lagN(1)..lagN(n)
  double mean = 0.0;
  for (int i = 1; i <= n; ++i) mean += engine.lagN(i);
  mean /= n;

  double var = 0.0;
  for (int i = 1; i <= n; ++i) {
    const double d = engine.lagN(i) - mean;
    var += d * d;
  }
  var = (n > 1) ? var / (n - 1) : 0.0;
  if (var < 1e-12) {
    out["spectral_acf1"] = 0.0;
    out["spectral_acf2"] = 0.0;
    out["spectral_period_hint"] = 0.0;
    return out;
  }

  auto acf = [&](int lag) -> double {
    if (lag >= n) return 0.0;
    double cov = 0.0;
    // lagN(1) = newest; lagN(k) = k-th newest
    for (int i = 1; i <= n - lag; ++i) {
      cov += (engine.lagN(i) - mean) * (engine.lagN(i + lag) - mean);
    }
    return cov / ((n - lag) * var);
  };

  const double acf1 = acf(1);
  const double acf2 = acf(2);
  double periodHint = 0.0;
  for (int lag = 2; lag < n; ++lag) {
    if (acf(lag) < 0.0) {
      periodHint = static_cast<double>(lag);
      break;
    }
  }

  out["spectral_acf1"] = acf1;
  out["spectral_acf2"] = acf2;
  out["spectral_period_hint"] = periodHint;
  return out;
}

} // namespace pe
