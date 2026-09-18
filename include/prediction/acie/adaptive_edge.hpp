#pragma once
/**
 * Phase 2 — adaptive edge / online feedback skeleton (TestingEngine-aligned).
 *
 * Outcomes update a rolling edge adjustment:
 *   - wins slightly lower required edge (more aggressive)
 *   - losses raise required edge (more selective)
 * Bounded and decayed so a streak cannot freeze the system.
 */
#include "validation/outcome.hpp"
#include "prediction/acie/strategy.hpp"
#include <algorithm>
#include <atomic>
#include <cmath>
#include <mutex>

namespace crashcore {
namespace acie {

struct AdaptiveEdgeConfig {
  double initialEdge = 0.02;
  double minEdge = 0.005;
  double maxEdge = 0.08;
  double winDelta = -0.001;   // after win, ease edge
  double lossDelta = +0.003;  // after loss, tighten
  double decayTowardInitial = 0.001; // per outcome mild mean-reversion
  std::size_t minOutcomesBeforeAdapt = 5;
};

class AdaptiveEdge {
public:
  explicit AdaptiveEdge(AdaptiveEdgeConfig cfg = {})
      : cfg_(std::move(cfg)), edge_(cfg_.initialEdge) {}

  double currentEdge() const {
    std::lock_guard lk(mu_);
    return edge_;
  }

  std::uint64_t outcomeCount() const noexcept { return outcomes_.load(); }
  std::uint64_t winCount() const noexcept { return wins_.load(); }
  std::uint64_t lossCount() const noexcept { return losses_.load(); }

  /** Apply exactly-once-style outcome feedback (caller must dedupe). */
  void noteOutcome(const Outcome& o) {
    if (o.result != PredictionOutcome::Win &&
        o.result != PredictionOutcome::Loss) {
      return; // ignore pending/void/stale/duplicate
    }
    std::lock_guard lk(mu_);
    ++outcomes_;
    if (o.isWin || o.result == PredictionOutcome::Win) {
      ++wins_;
      edge_ += cfg_.winDelta;
    } else {
      ++losses_;
      edge_ += cfg_.lossDelta;
    }
    // mild pull toward initial
    if (edge_ > cfg_.initialEdge) edge_ -= cfg_.decayTowardInitial;
    else if (edge_ < cfg_.initialEdge) edge_ += cfg_.decayTowardInitial;
    edge_ = std::clamp(edge_, cfg_.minEdge, cfg_.maxEdge);
  }

  /** Overlay adaptive edge onto a strategy config (quality mode). */
  StrategyConfig applyTo(StrategyConfig cfg) const {
    std::lock_guard lk(mu_);
    if (outcomes_.load() >= cfg_.minOutcomesBeforeAdapt) {
      cfg.qualityEdge = edge_;
    }
    return cfg;
  }

  AdaptiveEdgeConfig& config() noexcept { return cfg_; }

private:
  AdaptiveEdgeConfig cfg_;
  mutable std::mutex mu_;
  double edge_;
  std::atomic<std::uint64_t> outcomes_{0}, wins_{0}, losses_{0};
};

} // namespace acie
} // namespace crashcore
