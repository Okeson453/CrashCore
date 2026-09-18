#pragma once
/**
 * PORT_BEHAVIOR from TestingEngine src/lib/prediction/live/validator.ts
 * onGameEnd resolves pending predictions for completed round → WIN/LOSS,
 * feedback enqueue, stale/late handling. Does NOT call prediction models.
 */
#include "validation/prediction_registry.hpp"
#include "validation/outcome_matcher.hpp"
#include "validation/feedback_dispatcher.hpp"
#include "validation/win_loss.hpp"
#include "ingestion/crash_event.hpp"
#include "prediction_interface/prediction_event.hpp"
#include "timing/latency_tracker.hpp"
#include "timing/timestamp.hpp"
#include "common/result.hpp"
#include <functional>
#include <string>
#include <vector>
#include <atomic>
#include <mutex>
#include <unordered_set>
#include <list>
#include <unordered_map>

namespace crashcore {

enum class ValidateKind : std::uint8_t {
  Matched = 0,
  NoPending,
  BgArrivedLate,
  DuplicateEnd,
  Invalid
};

struct ValidateGameEndResult {
  ValidateKind kind = ValidateKind::Invalid;
  std::vector<Outcome> outcomes;
  std::string gameId;
  std::string detail;
};

class LiveValidator {
public:
  using OutcomeHandler = std::function<void(const Outcome&)>;
  using NextRoundHook = std::function<void(const std::string& completedGameId, double mult)>;

  explicit LiveValidator(LatencyTracker* tracker = nullptr)
      : registry_()
      , matcher_(registry_, tracker)
      , feedback_()
      , tracker_(tracker) {
    matcher_.setHandler([this](const Outcome& o) {
      feedback_.dispatch(o);
      if (on_outcome_) on_outcome_(o);
    });
  }

  void setOutcomeHandler(OutcomeHandler h) { on_outcome_ = std::move(h); }
  /** Optional hook after validation — CrashCore does not implement models. */
  void setNextRoundHook(NextRoundHook h) { on_next_ = std::move(h); }

  Result<void> registerPrediction(const PredictionEvent& ev) {
    return registry_.registerPrediction(ev);
  }

  /**
   * Spec §7.3: resolve pending predictions for the just-completed round.
   * Handles duplicate ED, late BG, and missing pending rows.
   */
  ValidateGameEndResult onGameEnd(const CrashEvent& endEvent) {
    ValidateGameEndResult result;
    const auto t0 = steadyNs();
    if (endEvent.kind != EventKind::End || !endEvent.valid) {
      result.kind = ValidateKind::Invalid;
      result.detail = "not a valid end event";
      return result;
    }
    const auto gameId = !endEvent.roundId.empty() ? endEvent.roundId : endEvent.gameId;
    result.gameId = gameId;
    if (gameId.empty()) {
      result.kind = ValidateKind::Invalid;
      result.detail = "missing gameId";
      return result;
    }

    {
      std::lock_guard lk(mu_);
      if (resolved_ends_.count(gameId)) {
        result.kind = ValidateKind::DuplicateEnd;
        result.detail = "duplicate end";
        ++duplicate_ends_;
        return result;
      }
      auto rit = resolved_map_.find(gameId);
      if (rit != resolved_map_.end()) {
        resolved_order_.splice(resolved_order_.end(), resolved_order_, rit->second);
      } else {
        resolved_order_.push_back(gameId);
        resolved_map_[gameId] = std::prev(resolved_order_.end());
        while (resolved_map_.size() > 5000) {
          const auto& oldest = resolved_order_.front();
          resolved_map_.erase(oldest);
          resolved_ends_.erase(oldest);
          resolved_order_.pop_front();
        }
      }
      resolved_ends_.insert(gameId);
    }

    auto outcomes = matcher_.match(endEvent);
    if (outcomes.empty()) {
      result.kind = ValidateKind::NoPending;
      result.detail = "no pending prediction";
      ++no_pending_;
    } else {
      result.kind = ValidateKind::Matched;
      result.outcomes = outcomes;
      for (const auto& o : outcomes) {
        stats_.record(o);
      }
      ++matched_rounds_;
    }

    // Non-blocking next-round hook (N+1 trigger boundary only)
    if (on_next_) {
      try {
        on_next_(gameId, endEvent.crashPoint);
      } catch (...) {
      }
    }

    if (tracker_) {
      tracker_->record(LatencyTracker::Stage::Validation, (steadyNs() - t0) / 1000);
    }
    return result;
  }

  /** Late BG after ED for same logical sequence — recovery marker. */
  void noteBgArrivedLate(const std::string& gameId) {
    std::lock_guard lk(mu_);
    late_bg_.insert(gameId);
    ++late_bg_count_;
  }

  bool wasBgLate(const std::string& gameId) const {
    std::lock_guard lk(mu_);
    return late_bg_.count(gameId) > 0;
  }

  PredictionRegistry& registry() noexcept { return registry_; }
  OutcomeMatcher& matcher() noexcept { return matcher_; }
  FeedbackDispatcher& feedback() noexcept { return feedback_; }
  const WinLossStats& stats() const noexcept { return stats_; }

  std::uint64_t matchedRounds() const noexcept { return matched_rounds_.load(); }
  std::uint64_t noPendingCount() const noexcept { return no_pending_.load(); }
  std::uint64_t duplicateEnds() const noexcept { return duplicate_ends_.load(); }
  std::uint64_t lateBgCount() const noexcept { return late_bg_count_.load(); }

  std::size_t expireStale(TimestampMs maxAgeMs = 300'000) {
    return registry_.expireStale(maxAgeMs);
  }

private:
  PredictionRegistry registry_;
  OutcomeMatcher matcher_;
  FeedbackDispatcher feedback_;
  LatencyTracker* tracker_ = nullptr;
  OutcomeHandler on_outcome_;
  NextRoundHook on_next_;
  WinLossStats stats_;
  mutable std::mutex mu_;
  std::unordered_set<std::string> resolved_ends_;
  std::list<std::string> resolved_order_;
  std::unordered_map<std::string, std::list<std::string>::iterator> resolved_map_;
  std::unordered_set<std::string> late_bg_;
  std::atomic<std::uint64_t> matched_rounds_{0};
  std::atomic<std::uint64_t> no_pending_{0};
  std::atomic<std::uint64_t> duplicate_ends_{0};
  std::atomic<std::uint64_t> late_bg_count_{0};
};

} // namespace crashcore
