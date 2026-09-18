#pragma once

/**
 * Per-feature contribution summary for diagnostics.
 * Port of feature-contribution.ts. Self-contained.
 */

#include <algorithm>
#include <cmath>
#include <string>
#include <unordered_map>
#include <vector>

namespace pe {

struct FeatureContribution {
  std::string feature;
  double value = 0.0;
  double weight = 0.0;
  double contribution = 0.0; // weight * (value - baseline) style
};

inline std::vector<FeatureContribution> rankContributions(
    const std::unordered_map<std::string, double>& values,
    const std::unordered_map<std::string, double>& weights,
    double /*baseline*/ = 0.0) {
  std::vector<FeatureContribution> out;
  for (const auto& [k, w] : weights) {
    const double v = values.count(k) ? values.at(k) : 0.0;
    out.push_back({k, v, w, w * v});
  }
  std::sort(out.begin(), out.end(),
            [](const FeatureContribution& a, const FeatureContribution& b) {
              return std::abs(a.contribution) > std::abs(b.contribution);
            });
  return out;
}

} // namespace pe
