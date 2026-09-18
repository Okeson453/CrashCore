#pragma once

#include "pe/types.hpp"
#include "pe/util/time.hpp"

#include <string>

namespace pe {

/**
 * Build the canonical immutable PredictionSignal from a PredictionOutput.
 * Callers must not mutate the returned signal.
 */
inline PredictionSignal toSignal(const PredictionOutput& out,
                                 FeaturePath featurePath,
                                 const std::string& targetRoundId) {
  PredictionSignal s;
  s.predictionId = out.predictionId;
  s.timestamp = out.timestamp;
  s.modelVersion = out.model.name + "@" + out.model.version;
  s.featureVersion = out.model.featureVersion;
  s.featurePath = featurePath;
  s.targetRoundId = targetRoundId;
  s.target = out.target;
  s.score = out.score;
  s.probability = out.probability;
  s.confidence = out.confidence;
  if (out.regime) s.regimeId = out.regime->id;
  s.dataQuality = out.dataQuality;
  s.reasoning = out.reasoning;
  s.expiresAt = out.expiresAt;
  s.featureSummary = out.featureSummary;
  return s;
}

} // namespace pe
