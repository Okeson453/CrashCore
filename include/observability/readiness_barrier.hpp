#pragma once
/**
 * 5-stage readiness barrier matching TE boot.ts HotReadinessStage.
 * BOOTING → MINIMAL_STATE_READY → PREDICTION_STATE_READY → LIVE_N1_READY → FULL_HYDRATION_READY
 * Events arriving before LIVE_N1_READY are held in a bounded buffer and drained on transition.
 */
#include "ingestion/crash_event.hpp"
#include <atomic>
#include <condition_variable>
#include <deque>
#include <functional>
#include <mutex>
#include <string>
#include <vector>

namespace crashcore {

enum class ReadinessStage : int {
  Booting = 0,
  MinimalStateReady = 1,
  PredictionStateReady = 2,
  LiveN1Ready = 3,
  FullHydrationReady = 4
};

inline const char* toString(ReadinessStage s) {
  switch (s) {
    case ReadinessStage::Booting: return "BOOTING";
    case ReadinessStage::MinimalStateReady: return "MINIMAL_STATE_READY";
    case ReadinessStage::PredictionStateReady: return "PREDICTION_STATE_READY";
    case ReadinessStage::LiveN1Ready: return "LIVE_N1_READY";
    case ReadinessStage::FullHydrationReady: return "FULL_HYDRATION_READY";
  }
  return "UNKNOWN";
}

class ReadinessBarrier {
public:
  using DrainHandler = std::function<void(const CrashEvent&)>;

  explicit ReadinessBarrier(std::size_t holdCapacity = 4096)
      : hold_capacity_(holdCapacity) {}

  ReadinessStage stage() const noexcept {
    return static_cast<ReadinessStage>(stage_.load(std::memory_order_acquire));
  }

  bool isLiveReady() const noexcept {
    return stage_.load(std::memory_order_acquire) >= static_cast<int>(ReadinessStage::LiveN1Ready);
  }

  bool advanceTo(ReadinessStage target) {
    int expected = stage_.load(std::memory_order_acquire);
    const int want = static_cast<int>(target);
    while (expected < want) {
      if (stage_.compare_exchange_weak(expected, want,
            std::memory_order_acq_rel, std::memory_order_acquire)) {
        if (target >= ReadinessStage::LiveN1Ready) {
          drainHeld();
        }
        cv_.notify_all();
        return true;
      }
    }
    return false;
  }

  /** Hold event if not yet LIVE_N1_READY; otherwise invoke handler immediately. */
  bool admit(const CrashEvent& ev, DrainHandler handler) {
    if (isLiveReady()) {
      if (handler) handler(ev);
      return true;
    }
    std::lock_guard lk(mu_);
    if (held_.size() >= hold_capacity_) {
      held_.pop_front();
      ++dropped_;
    }
    held_.push_back(ev);
    return false;
  }

  void setDrainHandler(DrainHandler h) {
    std::lock_guard lk(mu_);
    drain_handler_ = std::move(h);
  }

  void waitUntil(ReadinessStage s, std::chrono::milliseconds timeout = std::chrono::milliseconds(30000)) {
    std::unique_lock lk(mu_);
    cv_.wait_for(lk, timeout, [&] {
      return stage_.load(std::memory_order_acquire) >= static_cast<int>(s);
    });
  }

  std::size_t heldCount() const {
    std::lock_guard lk(mu_);
    return held_.size();
  }

  std::uint64_t droppedCount() const noexcept {
    return dropped_.load(std::memory_order_relaxed);
  }

private:
  void drainHeld() {
    std::vector<CrashEvent> batch;
    DrainHandler handler;
    {
      std::lock_guard lk(mu_);
      batch.assign(held_.begin(), held_.end());
      held_.clear();
      handler = drain_handler_;
    }
    if (handler) {
      for (const auto& ev : batch) handler(ev);
    }
  }

  std::size_t hold_capacity_;
  std::atomic<int> stage_{0};
  mutable std::mutex mu_;
  std::condition_variable cv_;
  std::deque<CrashEvent> held_;
  DrainHandler drain_handler_;
  std::atomic<std::uint64_t> dropped_{0};
};

} // namespace crashcore
