#pragma once
#include "common/types.hpp"
#include "timing/timestamp.hpp"

namespace crashcore {

// PredictionEvent is defined in common/types.hpp

inline PredictionEvent fromResponse(const PredictionResponse& resp, const GameId& gameId,
                                    double entryMult = 0.0) {
  PredictionEvent e;
  e.predictionId = resp.predictionId;
  e.targetRoundId = resp.targetRoundId;
  e.gameId = gameId;
  e.decision = resp.decision;
  e.confidence = resp.confidence;
  e.probability = resp.probability;
  e.entryMult = entryMult;
  e.targetMult = resp.targetMult;
  e.createdAtMs = resp.responseTimeMs ? resp.responseTimeMs : nowMs();
  e.createdAtNs = resp.responseNs ? resp.responseNs : steadyNs();
  e.correlationId = resp.correlationId;
  e.modelVersion = resp.modelVersion;
  return e;
}

} // namespace crashcore
