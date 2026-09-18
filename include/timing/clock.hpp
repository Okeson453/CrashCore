#pragma once
#include "common/types.hpp"
#include "timing/timestamp.hpp"
#include <atomic>

namespace crashcore {

class Clock {
public:
  Clock() = default;

  NanoTime now() const noexcept { return steadyNs(); }
  TimestampMs wallMs() const noexcept {
    return nowMs() + offsetMs_.load(std::memory_order_relaxed);
  }

  void setOffsetMs(std::int64_t offset) noexcept {
    offsetMs_.store(offset, std::memory_order_relaxed);
  }
  std::int64_t offsetMs() const noexcept {
    return offsetMs_.load(std::memory_order_relaxed);
  }

  void observeServerTime(TimestampMs serverMs) noexcept {
    const auto local = nowMs();
    const auto observed = serverMs - local;
    const auto prev = offsetMs_.load(std::memory_order_relaxed);
    const auto next = static_cast<std::int64_t>(0.9 * static_cast<double>(prev) + 0.1 * static_cast<double>(observed));
    offsetMs_.store(next, std::memory_order_relaxed);
  }

private:
  std::atomic<std::int64_t> offsetMs_{0};
};

} // namespace crashcore
