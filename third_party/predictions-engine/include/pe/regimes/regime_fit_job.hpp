#pragma once

/**
 * Offline regime fit job: scale + k-means on feature rows → cluster labels.
 * Port of regime-fit-job.ts. Uses learned_clustering only (no external deps).
 */

#include "pe/regimes/learned_clustering.hpp"
#include "pe/regimes/regime_state.hpp"

#include <string>
#include <vector>

namespace pe {

struct RegimeFitResult {
  ScalerParams scaler;
  std::vector<std::vector<double>> centroids;
  std::vector<int> assignments;
  int k = 0;
  std::string version = "regime-fit-v1";
};

inline RegimeFitResult fitRegimeClusters(
    const std::vector<std::vector<double>>& rows,
    int k = 8,
    int maxIter = 40) {
  RegimeFitResult result;
  result.k = k;
  result.scaler = fitStandardScaler(rows);
  std::vector<std::vector<double>> scaled;
  scaled.reserve(rows.size());
  for (const auto& r : rows) scaled.push_back(transform(r, result.scaler));
  auto km = kMeans(scaled, k, maxIter);
  result.centroids = std::move(km.centroids);
  result.assignments = std::move(km.assignments);
  return result;
}

} // namespace pe
