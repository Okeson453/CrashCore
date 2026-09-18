#pragma once
#include "common/types.hpp"
#include "timing/timestamp.hpp"

namespace crashcore {

// Outcome is defined in common/types.hpp

inline Outcome makeOutcome(const PredictionEvent& pred, double actualMult) {
  Outcome o;
  o.predictionId = pred.predictionId;
  o.roundId = pred.targetRoundId;
  o.actualMult = actualMult;
  o.targetMult = pred.targetMult;
  o.resolvedAtMs = nowMs();
  if (pred.decision != PredictionDecision::Enter) {
    o.result = PredictionOutcome::Void;
    o.isWin = false;
  } else if (actualMult >= pred.targetMult && pred.targetMult > 0) {
    o.result = PredictionOutcome::Win;
    o.isWin = true;
  } else {
    o.result = PredictionOutcome::Loss;
    o.isWin = false;
  }
  return o;
}

inline const char* outcomeName(PredictionOutcome o) noexcept {
  switch (o) {
    case PredictionOutcome::Pending:   return "pending";
    case PredictionOutcome::Win:       return "win";
    case PredictionOutcome::Loss:      return "loss";
    case PredictionOutcome::Void:      return "void";
    case PredictionOutcome::Stale:     return "stale";
    case PredictionOutcome::Duplicate: return "duplicate";
  }
  return "unknown";
}

} // namespace crashcore
