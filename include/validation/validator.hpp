#pragma once
/**
 * Top-level validation facade: registry + matcher + feedback dispatch hook.
 */
#include "validation/prediction_registry.hpp"
#include "validation/outcome_matcher.hpp"
#include "validation/feedback_dispatcher.hpp"
#include "ingestion/crash_event.hpp"
#include "prediction_interface/prediction_event.hpp"
#include "timing/latency_tracker.hpp"
#include "common/result.hpp"

namespace crashcore {

class Validator {
public:
  explicit Validator(LatencyTracker* tracker = nullptr)
      : registry_()
      , matcher_(registry_, tracker)
      , feedback_()
      , tracker_(tracker) {
    matcher_.setHandler([this](const Outcome& o) {
      feedback_.dispatch(o);
    });
  }

  Result<void> onPrediction(const PredictionEvent& ev) {
    return registry_.registerPrediction(ev);
  }

  std::vector<Outcome> onRoundEnd(const CrashEvent& endEvent) {
    return matcher_.match(endEvent);
  }

  PredictionRegistry& registry() noexcept { return registry_; }
  OutcomeMatcher& matcher() noexcept { return matcher_; }
  FeedbackDispatcher& feedback() noexcept { return feedback_; }

  std::size_t expireStale(TimestampMs maxAgeMs = 300'000) {
    return registry_.expireStale(maxAgeMs);
  }

private:
  PredictionRegistry registry_;
  OutcomeMatcher matcher_;
  FeedbackDispatcher feedback_;
  LatencyTracker* tracker_ = nullptr;
};

} // namespace crashcore
