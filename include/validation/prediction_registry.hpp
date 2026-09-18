#pragma once
/**
 * In-memory registry of outstanding predictions keyed by target round.
 */
#include "common/types.hpp"
#include "common/result.hpp"
#include "timing/timestamp.hpp"
#include <algorithm>
#include <unordered_map>
#include <mutex>
#include <vector>
#include <string>
#include <optional>

namespace crashcore {

class PredictionRegistry {
public:
  explicit PredictionRegistry(std::size_t max_entries = 10'000) : max_(max_entries) {}

  Result<void> registerPrediction(const PredictionEvent& ev) {
    if (ev.targetRoundId.empty()) {
      return Error{ErrorCode::InvalidArgument, "empty targetRoundId"};
    }
    std::lock_guard lk(mu_);
    auto& list = by_round_[ev.targetRoundId];
    for (const auto& existing : list) {
      if (existing.predictionId == ev.predictionId) {
        ++duplicates_;
        return Error{ErrorCode::Duplicate, "prediction already registered"};
      }
    }
    list.push_back(ev);
    by_id_[ev.predictionId] = ev.targetRoundId;
    ++registered_;
    pruneIfNeeded();
    return Result<void>::success();
  }

  std::vector<PredictionEvent> takeForRound(const RoundId& roundId) {
    std::lock_guard lk(mu_);
    auto it = by_round_.find(roundId);
    if (it == by_round_.end()) return {};
    auto out = std::move(it->second);
    for (const auto& e : out) by_id_.erase(e.predictionId);
    by_round_.erase(it);
    return out;
  }

  std::optional<PredictionEvent> find(const PredictionId& id) const {
    std::lock_guard lk(mu_);
    auto it = by_id_.find(id);
    if (it == by_id_.end()) return std::nullopt;
    auto rit = by_round_.find(it->second);
    if (rit == by_round_.end()) return std::nullopt;
    for (const auto& e : rit->second) {
      if (e.predictionId == id) return e;
    }
    return std::nullopt;
  }

  std::size_t size() const {
    std::lock_guard lk(mu_);
    std::size_t n = 0;
    for (const auto& [_, v] : by_round_) n += v.size();
    return n;
  }

  std::size_t expireStale(TimestampMs maxAgeMs) {
    std::lock_guard lk(mu_);
    const auto now = nowMs();
    std::size_t expired = 0;
    for (auto it = by_round_.begin(); it != by_round_.end(); ) {
      auto& list = it->second;
      list.erase(std::remove_if(list.begin(), list.end(),
          [&](const PredictionEvent& e) {
            if (now - e.createdAtMs > maxAgeMs) {
              by_id_.erase(e.predictionId);
              ++expired;
              return true;
            }
            return false;
          }), list.end());
      if (list.empty()) it = by_round_.erase(it);
      else ++it;
    }
    return expired;
  }

  std::uint64_t registeredCount() const noexcept { return registered_; }
  std::size_t pendingCount() const { return size(); }
  std::uint64_t duplicateCount() const noexcept { return duplicates_; }
  std::uint64_t prunedCount() const noexcept { return pruned_; }

private:
  void pruneIfNeeded() {
    std::size_t total = 0;
    for (const auto& [_, v] : by_round_) total += v.size();
    if (total <= max_) return;
    std::vector<std::pair<TimestampMs, std::string>> ages;
    for (const auto& [_, vec] : by_round_)
      for (const auto& e : vec) ages.emplace_back(e.createdAtMs, e.predictionId);
    std::sort(ages.begin(), ages.end());
    const std::size_t toDrop = total > max_ ? total - max_ : 0;
    for (std::size_t i = 0; i < toDrop && i < ages.size(); ++i) {
      const auto& pid = ages[i].second;
      auto it = by_id_.find(pid);
      if (it == by_id_.end()) continue;
      const auto roundId = it->second;
      by_id_.erase(it);
      auto rit = by_round_.find(roundId);
      if (rit != by_round_.end()) {
        auto& vec = rit->second;
        vec.erase(std::remove_if(vec.begin(), vec.end(),
          [&](const PredictionEvent& p){ return p.predictionId == pid; }), vec.end());
        if (vec.empty()) by_round_.erase(rit);
      }
      ++pruned_;
    }
  }

  std::size_t max_;
  mutable std::mutex mu_;
  std::unordered_map<RoundId, std::vector<PredictionEvent>> by_round_;
  std::unordered_map<PredictionId, RoundId> by_id_;
  std::uint64_t registered_ = 0;
  std::uint64_t duplicates_ = 0;
  std::uint64_t pruned_ = 0;
};

} // namespace crashcore
