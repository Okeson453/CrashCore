#pragma once
/**
 * Sliding-window duplicate event filter for crash frames.
 * Keys: gameId + event kind + coarse time bucket or sequence.
 */
#include "common/types.hpp"
#include "ingestion/crash_event.hpp"
#include <unordered_set>
#include <mutex>
#include <string>
#include <deque>
#include <utility>

namespace crashcore {

class DuplicateFilter {
public:
  explicit DuplicateFilter(std::size_t maxKeys = 8192) : max_(maxKeys) {}

  /** Returns true if this is the first time seeing the key (accept). */
  bool accept(const CrashEvent& ev) {
    if (!ev.valid) return false;
    const std::string key = makeKey(ev);
    std::lock_guard lk(mu_);
    if (seen_.count(key)) {
      ++duplicates_;
      return false;
    }
    seen_.insert(key);
    order_.push_back(key);
    while (order_.size() > max_) {
      seen_.erase(order_.front());
      order_.pop_front();
    }
    ++accepted_;
    return true;
  }

  bool acceptKey(const std::string& key) {
    std::lock_guard lk(mu_);
    if (seen_.count(key)) {
      ++duplicates_;
      return false;
    }
    seen_.insert(key);
    order_.push_back(key);
    while (order_.size() > max_) {
      seen_.erase(order_.front());
      order_.pop_front();
    }
    ++accepted_;
    return true;
  }

  static std::string makeKey(const CrashEvent& ev) {
    std::string id = !ev.gameId.empty() ? ev.gameId : ev.roundId;
    return id + ":" + std::to_string(static_cast<int>(ev.kind)) + ":" +
           std::to_string(ev.sequence) + ":" +
           std::to_string(ev.elapsedMs);
  }

  std::uint64_t acceptedCount() const noexcept { return accepted_; }
  std::uint64_t duplicateCount() const noexcept { return duplicates_; }
  std::size_t size() const {
    std::lock_guard lk(mu_);
    return seen_.size();
  }

  void clear() {
    std::lock_guard lk(mu_);
    seen_.clear();
    order_.clear();
  }

private:
  std::size_t max_;
  mutable std::mutex mu_;
  std::unordered_set<std::string> seen_;
  std::deque<std::string> order_;
  std::uint64_t accepted_ = 0;
  std::uint64_t duplicates_ = 0;
};

} // namespace crashcore
