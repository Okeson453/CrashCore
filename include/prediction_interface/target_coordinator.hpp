#pragma once
/**
 * PORT_CONTRACT from TestingEngine live/target-coordinator.ts
 * Tracks which rounds are eligible targets for the external PE.
 */
#include "common/types.hpp"
#include "timing/timestamp.hpp"
#include <mutex>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <optional>

namespace crashcore {

enum class TargetState : std::uint8_t {
  Available = 0,
  Claimed,
  Predicted,
  Resolved,
  Expired
};

struct TargetRecord {
  RoundId roundId;
  TargetState state = TargetState::Available;
  TimestampMs availableAtMs = 0;
  TimestampMs claimedAtMs = 0;
  TimestampMs resolvedAtMs = 0;
  PredictionId predictionId;
};

class TargetCoordinator {
public:
  void noteRoundStart(const RoundId& id) {
    std::lock_guard lk(mu_);
    auto& t = targets_[id];
    t.roundId = id;
    t.state = TargetState::Available;
    t.availableAtMs = nowMs();
  }

  bool claim(const RoundId& id, const PredictionId& predictionId) {
    std::lock_guard lk(mu_);
    auto it = targets_.find(id);
    if (it == targets_.end()) {
      TargetRecord t;
      t.roundId = id;
      t.state = TargetState::Claimed;
      t.claimedAtMs = nowMs();
      t.predictionId = predictionId;
      targets_[id] = t;
      ++claims_;
      return true;
    }
    if (it->second.state != TargetState::Available) return false;
    it->second.state = TargetState::Claimed;
    it->second.claimedAtMs = nowMs();
    it->second.predictionId = predictionId;
    ++claims_;
    return true;
  }

  void markPredicted(const RoundId& id) {
    std::lock_guard lk(mu_);
    auto it = targets_.find(id);
    if (it != targets_.end()) it->second.state = TargetState::Predicted;
  }

  /** Roll back a failed claim so the target can be retried. */
  bool release(const RoundId& id) {
    std::lock_guard lk(mu_);
    auto it = targets_.find(id);
    if (it == targets_.end()) return false;
    if (it->second.state == TargetState::Claimed ||
        it->second.state == TargetState::Predicted) {
      it->second.state = TargetState::Available;
      it->second.predictionId.clear();
      it->second.claimedAtMs = 0;
      ++releases_;
      return true;
    }
    return false;
  }

  void markResolved(const RoundId& id) {
    std::lock_guard lk(mu_);
    auto it = targets_.find(id);
    if (it != targets_.end()) {
      it->second.state = TargetState::Resolved;
      it->second.resolvedAtMs = nowMs();
      ++resolved_;
    }
  }

  std::optional<TargetRecord> get(const RoundId& id) const {
    std::lock_guard lk(mu_);
    auto it = targets_.find(id);
    if (it == targets_.end()) return std::nullopt;
    return it->second;
  }

  std::vector<RoundId> available() const {
    std::lock_guard lk(mu_);
    std::vector<RoundId> out;
    for (const auto& [id, t] : targets_)
      if (t.state == TargetState::Available) out.push_back(id);
    return out;
  }

  std::size_t expireOlderThan(TimestampMs ageMs) {
    const auto now = nowMs();
    std::lock_guard lk(mu_);
    std::size_t n = 0;
    for (auto& [id, t] : targets_) {
      if (t.state == TargetState::Available && t.availableAtMs > 0 &&
          now - t.availableAtMs > ageMs) {
        t.state = TargetState::Expired;
        ++n;
      }
    }
    expired_ += n;
    return n;
  }

  std::uint64_t claimCount() const noexcept { return claims_; }
  std::uint64_t resolvedCount() const noexcept { return resolved_; }

private:
  mutable std::mutex mu_;
  std::unordered_map<RoundId, TargetRecord> targets_;
  std::uint64_t claims_ = 0, resolved_ = 0, expired_ = 0, releases_ = 0;
};

} // namespace crashcore
