#pragma once
/**
 * PORT_BEHAVIOR from TestingEngine live/clock-offset.ts + clock-skew-monitor.ts
 */
#include "common/types.hpp"
#include "timing/timestamp.hpp"
#include <atomic>
#include <algorithm>
#include <cmath>
#include <vector>
#include <mutex>

namespace crashcore {

class ClockOffset {
public:
  TimestampMs authoritativeNowMs() const noexcept {
    return nowMs() + offset_ms_.load(std::memory_order_relaxed);
  }

  void observeServerTime(TimestampMs serverMs, TimestampMs localReceiveMs = 0) {
    if (localReceiveMs <= 0) localReceiveMs = nowMs();
    const auto sample = serverMs - localReceiveMs;
    std::lock_guard lk(mu_);
    samples_.push_back(sample);
    if (samples_.size() > 64) samples_.erase(samples_.begin());
    // median-ish: sort copy
    auto sorted = samples_;
    std::sort(sorted.begin(), sorted.end());
    const auto med = sorted[sorted.size() / 2];
    // EMA toward median
    const auto prev = offset_ms_.load(std::memory_order_relaxed);
    const auto next = static_cast<std::int64_t>(0.85 * prev + 0.15 * med);
    offset_ms_.store(next, std::memory_order_relaxed);
    ++observations_;
  }

  std::int64_t offsetMs() const noexcept {
    return offset_ms_.load(std::memory_order_relaxed);
  }

  std::int64_t skewAbsMs() const noexcept {
    return std::abs(offsetMs());
  }

  bool skewAlarm(std::int64_t thresholdMs = 2000) const noexcept {
    return skewAbsMs() > thresholdMs;
  }

  std::uint64_t observationCount() const noexcept { return observations_.load(); }

private:
  std::atomic<std::int64_t> offset_ms_{0};
  std::atomic<std::uint64_t> observations_{0};
  mutable std::mutex mu_;
  std::vector<std::int64_t> samples_;
};

} // namespace crashcore
