#pragma once
#include "common/types.hpp"
#include "validation/outcome.hpp"
#include "prediction_interface/prediction_event.hpp"
#include "timing/timestamp.hpp"
#include <sstream>
#include <iomanip>

namespace crashcore {

// Signal defined in common/types.hpp

inline Signal signalFromPrediction(const PredictionEvent& ev) {
  Signal s;
  s.kind = SignalKind::Prediction;
  s.predictionId = ev.predictionId;
  s.roundId = ev.targetRoundId;
  s.correlationId = ev.correlationId;
  s.createdAtMs = nowMs();
  s.urgent = ev.decision == PredictionDecision::Enter;
  std::ostringstream oss;
  oss << std::fixed << std::setprecision(2);
  if (ev.decision == PredictionDecision::Enter) {
    oss << "ENTER " << ev.targetRoundId
        << " target@" << ev.targetMult
        << " conf=" << ev.confidence;
  } else {
    oss << "SKIP " << ev.targetRoundId;
  }
  s.text = oss.str();
  return s;
}

inline Signal signalFromOutcome(const Outcome& o) {
  Signal s;
  s.kind = SignalKind::Outcome;
  s.predictionId = o.predictionId;
  s.roundId = o.roundId;
  s.createdAtMs = nowMs();
  std::ostringstream oss;
  oss << std::fixed << std::setprecision(2);
  oss << outcomeName(o.result) << " " << o.roundId
      << " actual=" << o.actualMult
      << " target=" << o.targetMult;
  s.text = oss.str();
  return s;
}

} // namespace crashcore
