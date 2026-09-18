#pragma once
#include "common/types.hpp"
#include <chrono>
#include <cmath>

namespace crashcore {

inline TimestampMs nowMs() noexcept {
  using namespace std::chrono;
  return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

inline NanoTime nowNs() noexcept {
  using namespace std::chrono;
  return duration_cast<nanoseconds>(steady_clock::now().time_since_epoch()).count();
}

inline NanoTime steadyNs() noexcept {
  using namespace std::chrono;
  return duration_cast<nanoseconds>(steady_clock::now().time_since_epoch()).count();
}

inline TimestampMs steadyMs() noexcept {
  using namespace std::chrono;
  return duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
}

inline double multiplierFromElapsed(std::int64_t elapsedMs) noexcept {
  if (elapsedMs <= 0) return 1.0;
  const double x = std::floor(100.0 * std::exp(6e-5 * static_cast<double>(elapsedMs))) / 100.0;
  return x < 1.0 ? 1.0 : x;
}

inline std::int64_t elapsedFromMultiplier(double multiplier) noexcept {
  if (multiplier <= 1.0) return 0;
  return static_cast<std::int64_t>(std::log(multiplier) / 6e-5);
}

} // namespace crashcore
