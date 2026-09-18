#pragma once
#include <deque>
/**
 * PORT_BEHAVIOR from TestingEngine src/lib/realtime/validator.ts
 * Validates normalized events: duplicates, stale, ordering, missing identity.
 */
#include "realtime/realtime_types.hpp"
#include "realtime/normalizer.hpp"
#include "timing/timestamp.hpp"
#include <mutex>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <optional>
#include <atomic>

namespace crashcore::realtime {

struct ValidateResult {
  bool ok = false;
  std::string reason;
  ValidatedRoundEvent event;
};

class EventValidator {
public:
  explicit EventValidator(std::int64_t maxSkewMs = 120'000, std::size_t maxSeen = 8192)
      : max_skew_ms_(maxSkewMs), max_seen_(maxSeen) {}

  ValidateResult validate(const NormalizedRoundEvent& n) {
    ValidateResult r;
    r.event = static_cast<ValidatedRoundEvent>(n);
    r.event.acceptedAt = nowMs();

    // TE validator.ts:28 — numeric-only gameId check first
    if (n.phase != RoundPhase::Progress) {
      if (n.gameId.empty()) {
        r.reason = "missing_game_id";
        ++invalid_;
        return r;
      }
      for (char c : n.gameId) {
        if (c < '0' || c > '9') {
          r.reason = "non_numeric_game_id";
          ++invalid_;
          return r;
        }
      }
    }

    // Stale: received far in the past relative to wall clock
    if (n.receivedAt > 0) {
      const auto lag = nowMs() - n.receivedAt;
      if (lag > max_skew_ms_) {
        r.reason = "stale";
        ++stale_;
        return r;
      }
      last_arrival_lag_ms_ = static_cast<double>(lag);
    }

    // Duplicate end/begin per gameId+phase
    if (n.phase == RoundPhase::End || n.phase == RoundPhase::Begin) {
      const std::string key = n.gameId + ":" + toString(n.phase);
      std::lock_guard lk(mu_);
      if (seen_.count(key)) {
        r.reason = "duplicate";
        ++duplicates_;
        return r;
      }
      seen_.insert(key);
      order_.push_back(key);
      while (order_.size() > max_seen_) {
        seen_.erase(order_.front());
        order_.pop_front();
      }
    }

    // Ordering: end before begin for same id is noted but still accepted (bg_arrived_late recovery)
    if (n.phase == RoundPhase::End) {
      std::lock_guard lk(mu_);
      // End may arrive before Begin (late recovery path); always record.
      last_phase_[n.gameId] = n.phase;
      last_mult_[n.gameId] = n.multiplier.value_or(0);
    } else if (n.phase == RoundPhase::Begin || n.phase == RoundPhase::Progress) {
      std::lock_guard lk(mu_);
      last_phase_[n.gameId] = n.phase;
      if (n.multiplier) last_mult_[n.gameId] = *n.multiplier;
    }

    r.ok = true;
    ++accepted_;
    return r;
  }

  std::uint64_t accepted() const noexcept { return accepted_.load(); }
  std::uint64_t duplicates() const noexcept { return duplicates_.load(); }
  std::uint64_t stale() const noexcept { return stale_.load(); }
  std::uint64_t invalid() const noexcept { return invalid_.load(); }
  double lastArrivalLagMs() const noexcept { return last_arrival_lag_ms_; }

private:
  std::int64_t max_skew_ms_;
  std::size_t max_seen_;
  mutable std::mutex mu_;
  std::unordered_set<std::string> seen_;
  std::deque<std::string> order_;
  std::unordered_map<std::string, RoundPhase> last_phase_;
  std::unordered_map<std::string, double> last_mult_;
  std::atomic<std::uint64_t> accepted_{0}, duplicates_{0}, stale_{0}, invalid_{0};
  double last_arrival_lag_ms_ = 0;
};

} // namespace crashcore::realtime
