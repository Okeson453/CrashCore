#pragma once
#include "timing/timestamp.hpp"
#include <atomic>
#include <cmath>
#include <deque>
#include <mutex>

namespace crashcore {

class ClockSkewMonitor {
public:
  explicit ClockSkewMonitor(std::size_t window = 64) : window_(window) {}
  void observe(TimestampMs serverMs) {
    if (serverMs <= 0) return;
    const double skew = static_cast<double>(nowMs()) - static_cast<double>(serverMs);
    std::lock_guard lk(mu_);
    samples_.push_back(skew);
    while (samples_.size() > window_) samples_.pop_front();
    double sum = 0, absSum = 0;
    for (double s : samples_) { sum += s; absSum += std::abs(s); }
    mean_.store(sum / samples_.size()); meanAbs_.store(absSum / samples_.size());
    ++observations_;
  }
  double meanSkewMs() const noexcept { return mean_.load(); }
  double meanAbsSkewMs() const noexcept { return meanAbs_.load(); }
  std::uint64_t observationCount() const noexcept { return observations_.load(); }
  bool isSkewed(double thresholdMs = 2000.0) const { return std::abs(meanSkewMs()) > thresholdMs; }
  TimestampMs correct(TimestampMs serverMs) const {
    return serverMs + static_cast<TimestampMs>(meanSkewMs());
  }
private:
  std::size_t window_;
  mutable std::mutex mu_;
  std::deque<double> samples_;
  std::atomic<double> mean_{0}, meanAbs_{0};
  std::atomic<std::uint64_t> observations_{0};
};

} // namespace crashcore
