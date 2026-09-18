#pragma once
#include "common/types.hpp"
#include "concurrency/spsc_queue.hpp"
#include "timing/timestamp.hpp"
#include <functional>
#include <atomic>
#include <mutex>
#include <thread>
#include <unordered_set>
#include <chrono>

namespace crashcore {

class FeedbackDispatcher {
public:
  using Handler = std::function<void(const Outcome&)>;

  explicit FeedbackDispatcher(std::size_t queue_capacity = 2048)
      : queue_(queue_capacity) {}
  ~FeedbackDispatcher() { stop(); }

  void setHandler(Handler h) { handler_ = std::move(h); }

  void start() {
    if (running_.exchange(true)) return;
    worker_ = std::jthread([this](std::stop_token st) {
      while (!st.stop_requested() && running_.load()) {
        auto item = queue_.try_pop();
        if (item) {
          const auto& o = *item;
          const std::string key = o.predictionId.empty() ? o.roundId : o.predictionId;
          if (!key.empty()) {
            std::lock_guard lk(dedupe_mu_);
            if (seen_.count(key)) { ++dupes_; continue; }
            seen_.insert(key);
            if (seen_.size() > 10000) seen_.clear();
          }
          if (handler_) handler_(o);
          ++handled_;
        } else {
          std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
      }
    });
  }

  void stop() {
    running_.store(false);
    if (worker_.joinable()) { worker_.request_stop(); worker_.join(); }
  }

  void dispatch(const Outcome& o) {
    ++dispatched_;
    if (handler_ && !running_.load()) { handler_(o); return; }
    if (!queue_.try_push(o)) ++drops_;
  }

  std::optional<Outcome> poll() { return queue_.try_pop(); }
  std::uint64_t dispatchedCount() const noexcept { return dispatched_.load(); }
  std::uint64_t dropCount() const noexcept { return drops_.load(); }
  std::uint64_t dupeCount() const noexcept { return dupes_.load(); }

private:
  Handler handler_;
  SpscQueue<Outcome> queue_;
  std::jthread worker_;
  std::atomic<bool> running_{false};
  std::mutex dedupe_mu_;
  std::unordered_set<std::string> seen_;
  std::atomic<std::uint64_t> dispatched_{0}, drops_{0}, dupes_{0}, handled_{0};
};

} // namespace crashcore
