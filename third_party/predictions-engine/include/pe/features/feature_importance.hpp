#pragma once

/**
 * Leave-one-group-out importance proxy (Brier delta). Port of feature-importance.ts.
 */

#include <cmath>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

namespace pe {

struct FeatureImportanceResult {
  std::string group;
  double baselineBrier = 0.0;
  double ablatedBrier = 0.0;
  double delta = 0.0;
};

inline double brierScore(const std::vector<double>& probs, const std::vector<double>& ys) {
  const size_t n = std::min(probs.size(), ys.size());
  if (n == 0) return 0.0;
  double s = 0.0;
  for (size_t i = 0; i < n; ++i) {
    const double d = probs[i] - ys[i];
    s += d * d;
  }
  return s / static_cast<double>(n);
}

/**
 * groups: group name → feature keys
 * scoreFn: feature map → probability
 */
inline std::vector<FeatureImportanceResult> computeGroupImportance(
    const std::vector<std::unordered_map<std::string, double>>& rows,
    const std::vector<double>& outcomes,
    const std::unordered_map<std::string, std::vector<std::string>>& groups,
    const std::function<double(const std::unordered_map<std::string, double>&)>& scoreFn) {
  std::vector<double> baseProbs;
  baseProbs.reserve(rows.size());
  for (const auto& r : rows) baseProbs.push_back(scoreFn(r));
  const double baseline = brierScore(baseProbs, outcomes);

  std::vector<FeatureImportanceResult> results;
  for (const auto& [group, keys] : groups) {
    std::vector<double> ablated;
    ablated.reserve(rows.size());
    for (const auto& r : rows) {
      auto copy = r;
      for (const auto& k : keys) copy[k] = 0.0;
      ablated.push_back(scoreFn(copy));
    }
    const double ab = brierScore(ablated, outcomes);
    results.push_back({group, baseline, ab, ab - baseline});
  }
  return results;
}

} // namespace pe
