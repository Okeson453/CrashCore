#pragma once
#include "delivery/outbox/outbox.hpp"
#include "delivery/telegram/telegram_client.hpp"
#include "timing/timestamp.hpp"
#include <atomic>
#include <thread>
#include <functional>
#include <string>

namespace crashcore {

/**
 * Background worker that claims outbox items and delivers via Telegram (or other sinks).
 * Must never run on the prediction hot path.
 */
class OutboxWorker {
public:
  using DeliverFn = std::function<bool(const OutboxItem&)>;

  OutboxWorker(Outbox& outbox, DeliverFn deliver)
      : outbox_(outbox), deliver_(std::move(deliver)) {}

  ~OutboxWorker() { stop(); }

  void start() {
    if (running_.exchange(true)) return;
    thread_ = std::jthread([this](std::stop_token st) { run(st); });
  }

  void stop() {
    running_.store(false);
    if (thread_.joinable()) {
      thread_.request_stop();
      thread_.join();
    }
  }

  bool running() const noexcept { return running_.load(); }
  std::uint64_t processed() const noexcept { return processed_.load(); }

private:
  void run(std::stop_token st) {
    while (!st.stop_requested() && running_.load()) {
      auto item = outbox_.claim(std::chrono::milliseconds(200));
      if (!item) continue;
      item->state = OutboxState::InFlight;
      bool ok = false;
      try {
        ok = deliver_ ? deliver_(*item) : false;
      } catch (...) {
        ok = false;
      }
      if (ok) {
        outbox_.markDelivered(*item);
      } else {
        outbox_.markFailed(*item, "delivery failed");
      }
      processed_.fetch_add(1, std::memory_order_relaxed);
    }
  }

  Outbox& outbox_;
  DeliverFn deliver_;
  std::atomic<bool> running_{false};
  std::atomic<std::uint64_t> processed_{0};
  std::jthread thread_;
};

} // namespace crashcore
