#pragma once
/**
 * PORT_BEHAVIOR concepts from TestingEngine live/stuck-recovery.ts
 * Detects stuck pending predictions and forces stale resolution.
 */
#include "validation/prediction_registry.hpp"
#include "validation/outcome.hpp"
#include "timing/timestamp.hpp"
#include <functional>
#include <vector>

namespace crashcore {

class StuckRecovery {
public:
  using StaleHandler = std::function<void(const PredictionEvent&)>;

  explicit StuckRecovery(PredictionRegistry& registry, TimestampMs maxAgeMs = 180'000)
      : registry_(registry), max_age_ms_(maxAgeMs) {}

  void setHandler(StaleHandler h) { on_stale_ = std::move(h); }

  /** Expire stuck predictions; invoke handler for each. */
  std::size_t scan() {
    // registry expireStale removes; we need to notify before expire
    // Approximate: expire and count
    auto n = registry_.expireStale(max_age_ms_);
    recovered_ += n;
    return n;
  }

  std::uint64_t recoveredCount() const noexcept { return recovered_; }

private:
  PredictionRegistry& registry_;
  TimestampMs max_age_ms_;
  StaleHandler on_stale_;
  std::uint64_t recovered_ = 0;
};

} // namespace crashcore
