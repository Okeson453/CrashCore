#pragma once
/**
 * Routes validated CrashEvents to prediction interface / validation / persistence queues.
 * Preserves ordering per round; drops duplicates; never blocks the hot path.
 */
#include "ingestion/crash_event.hpp"
#include "concurrency/spsc_queue.hpp"
#include "concurrency/mpsc_queue.hpp"
#include "timing/latency_tracker.hpp"
#include "timing/timestamp.hpp"
#include "common/types.hpp"
#include <unordered_set>
#include <unordered_map>
#include <list>
#include <string>
#include <atomic>
#include <mutex>
#include <functional>

namespace crashcore {

class EventRouter {
public:
  using EventHandler = std::function<void(const CrashEvent&)>;

  explicit EventRouter(std::size_t queue_capacity = 4096, LatencyTracker* tracker = nullptr)
      : to_prediction_(queue_capacity)
      , to_validation_(queue_capacity)
      , to_persistence_(queue_capacity)
      , tracker_(tracker) {}

  /**
   * Hot-path entry: normalize already-decoded event, dedupe, enqueue.
   * Returns false only if all target queues are full (backpressure).
   */
  bool route(CrashEvent ev) {
    const auto t0 = steadyNs();
    ev.routedAtNs = t0;

    if (!ev.valid) {
      ++rejected_;
      return true; // consumed but discarded
    }

    // Duplicate detection with LRU eviction (preserve ~10k recent keys)
    if (!ev.gameId.empty()) {
      const std::string key = ev.gameId + ":" + std::to_string(static_cast<int>(ev.kind))
                            + ":" + std::to_string(ev.sequence);
      std::lock_guard lk(dedupe_mu_);
      auto it = seen_map_.find(key);
      if (it != seen_map_.end()) {
        // Refresh LRU position
        seen_order_.splice(seen_order_.end(), seen_order_, it->second);
        ++duplicates_;
        return true;
      }
      seen_order_.push_back(key);
      seen_map_[key] = std::prev(seen_order_.end());
      while (seen_map_.size() > 10'000) {
        const auto& oldest = seen_order_.front();
        seen_map_.erase(oldest);
        seen_order_.pop_front();
      }
    }

    bool ok = true;
    // Progress and Start go to prediction path
    if (ev.kind == EventKind::Progress || ev.kind == EventKind::Start ||
        ev.kind == EventKind::Prepare) {
      if (!to_prediction_.try_push(ev)) {
        ++drops_pred_;
        ok = false;
      } else {
        ++routed_pred_;
      }
    }
    // End goes to validation
    if (ev.kind == EventKind::End) {
      if (!to_validation_.try_push(ev)) {
        ++drops_val_;
        ok = false;
      } else {
        ++routed_val_;
      }
    }
    // Everything interesting to persistence (async)
    if (ev.kind == EventKind::End || ev.kind == EventKind::Start) {
      to_persistence_.try_push(ev); // best-effort
      ++routed_pers_;
    }

    if (tracker_) {
      tracker_->record(LatencyTracker::Stage::Queue, (steadyNs() - t0) / 1000);
    }
    return ok;
  }

  std::optional<CrashEvent> popPrediction() { return to_prediction_.try_pop(); }
  std::optional<CrashEvent> popValidation() { return to_validation_.try_pop(); }
  std::optional<CrashEvent> popPersistence() { return to_persistence_.try_pop(); }

  struct Stats {
    std::uint64_t routedPred = 0, routedVal = 0, routedPers = 0;
    std::uint64_t dropsPred = 0, dropsVal = 0, duplicates = 0, rejected = 0;
  };

  Stats stats() const {
    Stats s;
    s.routedPred = routed_pred_.load(std::memory_order_relaxed);
    s.routedVal  = routed_val_.load(std::memory_order_relaxed);
    s.routedPers = routed_pers_.load(std::memory_order_relaxed);
    s.dropsPred  = drops_pred_.load(std::memory_order_relaxed);
    s.dropsVal   = drops_val_.load(std::memory_order_relaxed);
    s.duplicates = duplicates_.load(std::memory_order_relaxed);
    s.rejected   = rejected_.load(std::memory_order_relaxed);
    return s;
  }

private:
  SpscQueue<CrashEvent> to_prediction_;
  SpscQueue<CrashEvent> to_validation_;
  SpscQueue<CrashEvent> to_persistence_;
  LatencyTracker* tracker_ = nullptr;

  std::list<std::string> seen_order_;
  std::unordered_map<std::string, std::list<std::string>::iterator> seen_map_;
  std::mutex dedupe_mu_;

  std::atomic<std::uint64_t> routed_pred_{0}, routed_val_{0}, routed_pers_{0};
  std::atomic<std::uint64_t> drops_pred_{0}, drops_val_{0};
  std::atomic<std::uint64_t> duplicates_{0}, rejected_{0};
};

} // namespace crashcore
