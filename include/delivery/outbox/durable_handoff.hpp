#pragma once
/**
 * PORT_BEHAVIOR from TestingEngine durable-handoff-ordering tests
 * Ensures prediction signal is published before outcome signal for same id.
 */
#include "delivery/outbox/outbox.hpp"
#include "delivery/signal.hpp"
#include "common/result.hpp"
#include <mutex>
#include <string>
#include <unordered_set>

namespace crashcore {

class DurableHandoff {
public:
  explicit DurableHandoff(Outbox& outbox) : outbox_(outbox) {}

  Result<std::uint64_t> publishPrediction(Signal sig) {
    sig.kind = SignalKind::Prediction;
    auto r = outbox_.publish(sig);
    if (r) {
      std::lock_guard lk(mu_);
      pred_published_.insert(sig.predictionId);
    }
    return r;
  }

  Result<std::uint64_t> publishOutcome(Signal sig) {
    sig.kind = SignalKind::Outcome;
    {
      std::lock_guard lk(mu_);
      // ordering preference: prediction first (not hard-fail if missing)
      if (!pred_published_.count(sig.predictionId)) ++out_of_order_;
    }
    return outbox_.publish(sig);
  }

  bool hasPredictionPublished(const std::string& predictionId) const {
    std::lock_guard lk(mu_);
    return pred_published_.count(predictionId) > 0;
  }

  std::uint64_t outOfOrderCount() const noexcept { return out_of_order_; }

private:
  Outbox& outbox_;
  mutable std::mutex mu_;
  std::unordered_set<std::string> pred_published_;
  std::uint64_t out_of_order_ = 0;
};

} // namespace crashcore
