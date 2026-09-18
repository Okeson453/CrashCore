#pragma once
#include "common/types.hpp"

namespace crashcore {

struct WinLossStats {
  std::uint64_t wins = 0;
  std::uint64_t losses = 0;
  std::uint64_t voids = 0;
  std::uint64_t stale = 0;
  double        sumConfidence = 0.0;

  double winRate() const noexcept {
    const auto n = wins + losses;
    return n == 0 ? 0.0 : static_cast<double>(wins) / static_cast<double>(n);
  }

  void record(const Outcome& o, double confidence = 0.0) {
    sumConfidence += confidence;
    switch (o.result) {
      case PredictionOutcome::Win:  ++wins; break;
      case PredictionOutcome::Loss: ++losses; break;
      case PredictionOutcome::Void: ++voids; break;
      case PredictionOutcome::Stale: ++stale; break;
      default: break;
    }
  }
};

} // namespace crashcore
