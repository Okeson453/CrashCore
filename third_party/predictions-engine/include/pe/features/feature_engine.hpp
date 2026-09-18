#pragma once

/**
 * FeatureEngine V1 — full-history scan path (fallback when incremental is cold).
 * Port of feature-engine.ts. Self-contained.
 */

#include "pe/features/calculators.hpp"
#include "pe/features/feature_meta.hpp"
#include "pe/types.hpp"
#include "pe/util/time.hpp"

#include <cmath>
#include <string>
#include <vector>

namespace pe {

class FeatureEngine {
public:
  explicit FeatureEngine(std::string featureVersion = FEATURE_VERSION_V2)
    : featureVersion_(std::move(featureVersion)) {}

  const std::string& featureVersion() const { return featureVersion_; }

  FeatureVector buildVector(const std::vector<HistoricalRound>& priorRounds,
                            const std::string& targetRoundId,
                            const std::string& timestamp) const {
    auto values = computeFeatures(priorRounds, timestamp);
    int missing = 0;
    for (auto& [k, v] : values) {
      if (!std::isfinite(v)) { v = 0.0; ++missing; }
    }
    FeatureVector fv;
    fv.roundId = targetRoundId;
    fv.timestamp = timestamp;
    fv.featureVersion = featureVersion_;
    fv.values = std::move(values);
    fv.meta.sampleSize = static_cast<int>(priorRounds.size());
    fv.meta.dataQualityScore = fv.values.count("quality_score") ? fv.values.at("quality_score") : 0.0;
    fv.meta.missingFeatureCount = missing;
    return fv;
  }

  std::vector<FeatureVector> buildSequence(const std::vector<HistoricalRound>& rounds,
                                           int minHistory = 20) const {
    std::vector<FeatureVector> vectors;
    for (int i = minHistory; i < static_cast<int>(rounds.size()); ++i) {
      std::vector<HistoricalRound> prior(rounds.begin(), rounds.begin() + i);
      const auto& target = rounds[static_cast<size_t>(i)];
      const std::string ts = target.startedAt.value_or(
          target.crashedAt.value_or(target.createdAt));
      vectors.push_back(buildVector(prior, target.id, ts));
    }
    return vectors;
  }

private:
  std::string featureVersion_;
};

} // namespace pe
