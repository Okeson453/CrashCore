#pragma once
#include "validation/prediction_registry.hpp"
#include "validation/outcome.hpp"
#include "validation/win_loss.hpp"
#include "ingestion/crash_event.hpp"
#include "timing/latency_tracker.hpp"
#include "timing/timestamp.hpp"
#include <vector>
#include <functional>
#include <atomic>
namespace crashcore {
class OutcomeMatcher {
public:
  using OutcomeHandler = std::function<void(const Outcome&)>;
  explicit OutcomeMatcher(PredictionRegistry& registry, LatencyTracker* tracker = nullptr)
      : registry_(registry), tracker_(tracker) {}
  void setHandler(OutcomeHandler h) { handler_ = std::move(h); }
  std::vector<Outcome> match(const CrashEvent& endEvent) {
    const auto t0 = steadyNs();
    std::vector<Outcome> results;
    if (endEvent.kind != EventKind::End || !endEvent.valid) return results;
    const auto roundId = endEvent.roundId.empty() ? endEvent.gameId : endEvent.roundId;
    auto preds = registry_.takeForRound(roundId);
    for (const auto& p : preds) {
      auto o = makeOutcome(p, endEvent.crashPoint);
      if (o.result == PredictionOutcome::Void) ++voids_;
      else if (o.result == PredictionOutcome::Stale) ++stales_;
      else stats_.record(o, p.confidence);
      results.push_back(o);
      if (handler_) handler_(o);
      ++matched_;
    }
    if (preds.empty()) ++unmatched_ends_;
    if (tracker_) tracker_->record(LatencyTracker::Stage::Validation, (steadyNs() - t0) / 1000);
    return results;
  }
  const WinLossStats& stats() const noexcept { return stats_; }
  std::uint64_t matchedCount() const noexcept { return matched_.load(); }
  std::uint64_t unmatchedEnds() const noexcept { return unmatched_ends_.load(); }
  std::uint64_t voidCount() const noexcept { return voids_.load(); }
  std::uint64_t staleCount() const noexcept { return stales_.load(); }
private:
  PredictionRegistry& registry_;
  LatencyTracker* tracker_ = nullptr;
  OutcomeHandler handler_;
  WinLossStats stats_;
  std::atomic<std::uint64_t> matched_{0}, unmatched_ends_{0}, voids_{0}, stales_{0};
};
} // namespace crashcore
