#pragma once
/**
 * PORT_BEHAVIOR boundary from TestingEngine prediction-loss-cooldown.ts
 * Tracks consecutive losses / cooldown windows for feedback to external PE.
 * CrashCore does not suppress model inference — only records cooldown state
 * for the interface contract.
 */
#include "common/types.hpp"
#include "validation/outcome.hpp"
#include "timing/timestamp.hpp"
#include <atomic>
#include <mutex>
#include <string>

namespace crashcore {

struct CooldownState {
  int consecutiveLosses = 0;
  int consecutiveWins = 0;
  TimestampMs lastLossMs = 0;
  TimestampMs lastWinMs = 0;
  TimestampMs cooldownUntilMs = 0;
  bool active = false;
};

class LossCooldownTracker {
public:
  explicit LossCooldownTracker(int lossThreshold = 3, std::int64_t cooldownMs = 60'000)
      : loss_threshold_(lossThreshold), cooldown_ms_(cooldownMs) {}

  void noteOutcome(const Outcome& o) {
    std::lock_guard lk(mu_);
    if (o.result == PredictionOutcome::Win) {
      state_.consecutiveWins += 1;
      state_.consecutiveLosses = 0;
      state_.lastWinMs = o.resolvedAtMs ? o.resolvedAtMs : nowMs();
      state_.active = false;
      state_.cooldownUntilMs = 0;
      ++wins_;
    } else if (o.result == PredictionOutcome::Loss) {
      state_.consecutiveLosses += 1;
      state_.consecutiveWins = 0;
      state_.lastLossMs = o.resolvedAtMs ? o.resolvedAtMs : nowMs();
      ++losses_;
      if (state_.consecutiveLosses >= loss_threshold_) {
        state_.active = true;
        state_.cooldownUntilMs = state_.lastLossMs + cooldown_ms_;
        ++activations_;
      }
    }
  }

  void noteRoundCompleted() {
    // used by external PE; CrashCore only counts
    ++rounds_;
  }

  bool inCooldown(TimestampMs now = 0) const {
    if (now <= 0) now = nowMs();
    std::lock_guard lk(mu_);
    return state_.active && now < state_.cooldownUntilMs;
  }

  CooldownState state() const {
    std::lock_guard lk(mu_);
    return state_;
  }

  std::uint64_t activationCount() const noexcept { return activations_.load(); }
  std::uint64_t winCount() const noexcept { return wins_.load(); }
  std::uint64_t lossCount() const noexcept { return losses_.load(); }

private:
  int loss_threshold_;
  std::int64_t cooldown_ms_;
  mutable std::mutex mu_;
  CooldownState state_;
  std::atomic<std::uint64_t> activations_{0}, wins_{0}, losses_{0}, rounds_{0};
};

} // namespace crashcore
