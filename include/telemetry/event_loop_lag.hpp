#pragma once
/**
 * PORT_BEHAVIOR proxy for TestingEngine event-loop-lag.ts
 * Measures scheduling lag of a periodic tick (not Node event loop).
 */
#include "timing/monotonic_clock.hpp"
#include "timing/timestamp.hpp"
#include <atomic>
#include <algorithm>
#include <vector>
#include <mutex>

namespace crashcore {

class EventLoopLagMonitor {
public:
  explicit EventLoopLagMonitor(std::int64_t expectedIntervalUs = 5000)
      : expected_us_(expectedIntervalUs) {}

  void tick() {
    const auto now = MonotonicClock::now();
    if (has_last_) {
      auto actual = MonotonicClock::elapsedUs(last_, now);
      auto lag = actual > expected_us_ ? actual - expected_us_ : 0;
      std::lock_guard lk(mu_);
      samples_.push_back(lag);
      if (samples_.size() > 256) samples_.erase(samples_.begin());
      last_lag_us_ = lag;
      if (lag > max_lag_us_) max_lag_us_ = lag;
    }
    last_ = now;
    has_last_ = true;
    ++ticks_;
  }

  std::int64_t lastLagUs() const noexcept { return last_lag_us_; }
  std::int64_t maxLagUs() const noexcept { return max_lag_us_; }

  double avgLagUs() const {
    std::lock_guard lk(mu_);
    if (samples_.empty()) return 0;
    double s = 0;
    for (auto v : samples_) s += static_cast<double>(v);
    return s / static_cast<double>(samples_.size());
  }

  std::uint64_t tickCount() const noexcept { return ticks_.load(); }

private:
  std::int64_t expected_us_;
  MonotonicClock::TimePoint last_{};
  bool has_last_ = false;
  std::atomic<std::uint64_t> ticks_{0};
  std::int64_t last_lag_us_ = 0;
  std::int64_t max_lag_us_ = 0;
  mutable std::mutex mu_;
  std::vector<std::int64_t> samples_;
};

} // namespace crashcore
