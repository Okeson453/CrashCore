#pragma once
/**
 * PORT_CONTRACT + PORT_BEHAVIOR from TestingEngine src/lib/prediction/live/feedback.ts
 * CrashCore dispatches feedback payloads; learning remains external.
 */
#include "common/types.hpp"
#include "validation/outcome.hpp"
#include "timing/timestamp.hpp"
#include <string>
#include <optional>
#include <vector>
#include <cstdint>

namespace crashcore {

enum class FeedbackKind : std::uint8_t {
  OutcomeResolved = 0,
  PredictionSkipped,
  RoundMissed,
  StaleExpired
};

struct FeedbackPayload {
  FeedbackKind kind = FeedbackKind::OutcomeResolved;
  PredictionId predictionId;
  RoundId roundId;
  PredictionOutcome result = PredictionOutcome::Pending;
  double actualMult = 0.0;
  double targetMult = 0.0;
  double confidence = 0.0;
  TimestampMs resolvedAtMs = 0;
  std::string correlationId;
  std::string modelVersion;
  bool isWin = false;
};

inline FeedbackPayload feedbackFromOutcome(const Outcome& o,
                                           double confidence = 0.0,
                                           std::string correlationId = {},
                                           std::string modelVersion = {}) {
  FeedbackPayload f;
  f.kind = FeedbackKind::OutcomeResolved;
  f.predictionId = o.predictionId;
  f.roundId = o.roundId;
  f.result = o.result;
  f.actualMult = o.actualMult;
  f.targetMult = o.targetMult;
  f.confidence = confidence;
  f.resolvedAtMs = o.resolvedAtMs ? o.resolvedAtMs : nowMs();
  f.correlationId = std::move(correlationId);
  f.modelVersion = std::move(modelVersion);
  f.isWin = o.isWin;
  return f;
}

inline const char* toString(FeedbackKind k) noexcept {
  switch (k) {
    case FeedbackKind::OutcomeResolved: return "outcome_resolved";
    case FeedbackKind::PredictionSkipped: return "prediction_skipped";
    case FeedbackKind::RoundMissed: return "round_missed";
    case FeedbackKind::StaleExpired: return "stale_expired";
  }
  return "unknown";
}

} // namespace crashcore
