#pragma once
#include "common/types.hpp"
#include <chrono>

namespace crashcore {

class MonotonicClock {
public:
  using clock = std::chrono::steady_clock;
  using time_point = clock::time_point;

  static time_point now() noexcept { return clock::now(); }

  static NanoTime toNs(time_point tp) noexcept {
    return std::chrono::duration_cast<std::chrono::nanoseconds>(tp.time_since_epoch()).count();
  }

  static TimestampMs toMs(time_point tp) noexcept {
    return std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch()).count();
  }

  static std::int64_t elapsedUs(time_point start, time_point end) noexcept {
    return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
  }

  static std::int64_t elapsedNs(time_point start, time_point end) noexcept {
    return std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
  }
};

} // namespace crashcore
