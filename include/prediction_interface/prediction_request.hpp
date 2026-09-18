#pragma once
#include "common/types.hpp"
#include "timing/timestamp.hpp"
#include <string>

namespace crashcore {

// PredictionRequest is defined in common/types.hpp

inline PredictionRequest makePredictionRequest(const CrashEvent& ev, std::string correlationId = {}) {
  PredictionRequest r;
  r.targetRoundId = ev.roundId.empty() ? ev.gameId : ev.roundId;
  r.gameId = ev.gameId;
  r.requestTimeMs = nowMs();
  r.requestNs = steadyNs();
  r.sequence = ev.sequence;
  r.currentMult = ev.currentMult > 0 ? ev.currentMult : ev.crashPoint;
  r.correlationId = std::move(correlationId);
  if (r.correlationId.empty()) {
    r.correlationId = r.targetRoundId + "-" + std::to_string(r.requestTimeMs);
  }
  return r;
}

} // namespace crashcore
