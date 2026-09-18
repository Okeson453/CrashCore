#pragma once

/**
 * Hard latency budget for the prediction critical path.
 *
 * Target: 1 ms typical, 5 ms hard ceiling (p99 / max under load).
 * The V2 incremental path (state update + features + regime + model + signal)
 * must stay inside this envelope; no full-history scans, no I/O, no heap
 * storms on the hot path.
 */

#include <chrono>
#include <cstdint>

namespace pe {

/** Soft target for a single predict() call (microseconds). */
inline constexpr int64_t LATENCY_TARGET_US = 1'000;   // 1 ms

/** Hard ceiling — treat as SLO breach if exceeded (microseconds). */
inline constexpr int64_t LATENCY_BUDGET_US = 5'000;   // 5 ms

/** State update alone should be well under this. */
inline constexpr int64_t LATENCY_UPDATE_BUDGET_US = 100; // 0.1 ms

class LatencyTimer {
public:
  LatencyTimer() : start_(std::chrono::steady_clock::now()) {}

  /** Elapsed microseconds since construction. */
  int64_t elapsedUs() const {
    using namespace std::chrono;
    return duration_cast<microseconds>(steady_clock::now() - start_).count();
  }

  bool withinTarget() const { return elapsedUs() <= LATENCY_TARGET_US; }
  bool withinBudget() const { return elapsedUs() <= LATENCY_BUDGET_US; }

private:
  std::chrono::steady_clock::time_point start_;
};

} // namespace pe
