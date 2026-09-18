#pragma once
/**
 * Per-source sequence tracking for gap detection (missed events).
 */
#include "common/types.hpp"
#include <mutex>
#include <string>
#include <unordered_map>
#include <cstdint>

namespace crashcore {

class SequenceTracker {
public:
  /** Returns number of missed sequences if gap detected; 0 otherwise. */
  std::uint64_t observe(const std::string& source, SequenceNum seq) {
    std::lock_guard lk(mu_);
    auto& last = last_[source];
    if (!initialized_[source]) {
      last = seq;
      initialized_[source] = true;
      return 0;
    }
    std::uint64_t missed = 0;
    if (seq > last + 1) {
      missed = seq - last - 1;
      missed_total_ += missed;
    }
    if (seq > last) last = seq;
    return missed;
  }

  std::uint64_t missedTotal() const noexcept { return missed_total_; }

  SequenceNum last(const std::string& source) const {
    std::lock_guard lk(mu_);
    auto it = last_.find(source);
    return it == last_.end() ? 0 : it->second;
  }

  void reset(const std::string& source) {
    std::lock_guard lk(mu_);
    last_.erase(source);
    initialized_.erase(source);
  }

  void resetAll() {
    std::lock_guard lk(mu_);
    last_.clear();
    initialized_.clear();
    missed_total_ = 0;
  }

  std::size_t sourceCount() const {
    std::lock_guard lk(mu_);
    return last_.size();
  }

private:
  mutable std::mutex mu_;
  std::unordered_map<std::string, SequenceNum> last_;
  std::unordered_map<std::string, bool> initialized_;
  std::uint64_t missed_total_ = 0;
};

} // namespace crashcore
