#pragma once
/**
 * PORT_BEHAVIOR runtime invariants from TestingEngine live/invariants.ts concepts.
 * Checked off hot path or at stage boundaries.
 */
#include "common/types.hpp"
#include "ingestion/crash_event.hpp"
#include "prediction_interface/prediction_event.hpp"
#include "validation/outcome.hpp"
#include <string>
#include <vector>
#include <atomic>

namespace crashcore {

enum class InvariantId : std::uint16_t {
  EndRequiresGameId = 1,
  PredictionRequiresTarget,
  OutcomeRequiresPrediction,
  WinImpliesActualGteTarget,
  LossImpliesActualLtTarget,
  NoDuplicatePredictionId,
  SequenceNonDecreasing,
  COUNT
};

struct InvariantViolation {
  InvariantId id;
  std::string message;
  std::string context;
};

class InvariantChecker {
public:
  std::vector<InvariantViolation> checkEndEvent(const CrashEvent& e) {
    std::vector<InvariantViolation> v;
    if (e.kind == EventKind::End) {
      if (e.gameId.empty() && e.roundId.empty()) {
        v.push_back({InvariantId::EndRequiresGameId, "end without gameId", ""});
        ++violations_;
      }
    }
    return v;
  }

  std::vector<InvariantViolation> checkPrediction(const PredictionEvent& e) {
    std::vector<InvariantViolation> v;
    if (e.targetRoundId.empty()) {
      v.push_back({InvariantId::PredictionRequiresTarget, "empty targetRoundId", e.predictionId});
      ++violations_;
    }
    if (e.predictionId.empty()) {
      v.push_back({InvariantId::NoDuplicatePredictionId, "empty predictionId", ""});
      ++violations_;
    }
    return v;
  }

  std::vector<InvariantViolation> checkOutcome(const Outcome& o) {
    std::vector<InvariantViolation> v;
    if (o.predictionId.empty()) {
      v.push_back({InvariantId::OutcomeRequiresPrediction, "empty predictionId", o.roundId});
      ++violations_;
    }
    if (o.result == PredictionOutcome::Win) {
      if (o.targetMult > 0 && o.actualMult < o.targetMult) {
        v.push_back({InvariantId::WinImpliesActualGteTarget,
                     "WIN but actual < target", o.predictionId});
        ++violations_;
      }
    }
    if (o.result == PredictionOutcome::Loss) {
      if (o.targetMult > 0 && o.actualMult >= o.targetMult) {
        v.push_back({InvariantId::LossImpliesActualLtTarget,
                     "LOSS but actual >= target", o.predictionId});
        ++violations_;
      }
    }
    return v;
  }

  std::uint64_t violationCount() const noexcept { return violations_.load(); }

private:
  std::atomic<std::uint64_t> violations_{0};
};

} // namespace crashcore
