#pragma once
#include "persistence/outbox_repository.hpp"
#include "delivery/telegram/telegram_client.hpp"
#include "common/result.hpp"
#include "timing/timestamp.hpp"
#include <atomic>
#include <chrono>
#include <functional>
#include <random>
#include <string>
#include <thread>

namespace crashcore {

class NotificationWorker {
public:
  struct Config {
    std::string workerId = "nw-1";
    std::size_t batchSize = 16;
    std::int64_t pollIntervalMs = 200;
    std::int64_t claimStaleMs = 30000;
    int maxAttempts = 8;
    std::int64_t baseRetryMs = 500;
    std::int64_t maxRetryMs = 60000;
  };
  using DeliverFn = std::function<Result<void>(const OutboxRow&)>;

  NotificationWorker(OutboxRepository& repo, TelegramClient* telegram = nullptr)
      : repo_(repo), telegram_(telegram) {}
  NotificationWorker(OutboxRepository& repo, TelegramClient* telegram, Config cfg)
      : repo_(repo), telegram_(telegram), cfg_(std::move(cfg)) {}

  void setDeliverFn(DeliverFn fn) { deliver_ = std::move(fn); }
  void start() {
    if (running_.exchange(true)) return;
    worker_ = std::jthread([this](std::stop_token st) { loop(st); });
  }
  void stop() {
    running_.store(false);
    if (worker_.joinable()) { worker_.request_stop(); worker_.join(); }
  }
  bool running() const noexcept { return running_.load(); }
  std::uint64_t delivered() const noexcept { return delivered_.load(); }
  std::uint64_t failed() const noexcept { return failed_.load(); }
  std::size_t tickOnce() { return drainBatch(); }

private:
  void loop(std::stop_token st) {
    std::mt19937 rng{std::random_device{}()};
    std::uniform_int_distribution<std::int64_t> jitter(0, 50);
    while (!st.stop_requested() && running_.load()) {
      if ((loops_++ % 50) == 0) repo_.recoverStaleClaims(cfg_.claimStaleMs);
      drainBatch();
      const auto sleepMs = cfg_.pollIntervalMs + jitter(rng);
      for (std::int64_t w = 0; w < sleepMs && !st.stop_requested(); w += 20)
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
  }
  std::size_t drainBatch() {
    auto claimed = repo_.claim(cfg_.batchSize, cfg_.workerId);
    if (!claimed) return 0;
    std::size_t n = 0;
    for (const auto& row : claimed.value()) {
      auto r = doDeliver(row);
      if (r) { repo_.markDelivered(row.id); ++delivered_; ++n; }
      else {
        const bool dead = row.attempts >= cfg_.maxAttempts;
        const auto retry = std::min<std::int64_t>(cfg_.maxRetryMs,
            cfg_.baseRetryMs * (std::int64_t{1} << std::min(row.attempts, 10)));
        repo_.markFailed(row.id, r.error().message, retry, dead);
        if (dead) ++dead_; else ++failed_;
      }
    }
    return n;
  }
  Result<void> doDeliver(const OutboxRow& row) {
    if (deliver_) return deliver_(row);
    if (telegram_) {
      std::string text = row.payload;
      auto r = telegram_->sendMessageWithRetry(text);
      if (!r) return Error{ErrorCode::NetworkError, r.error().message};
      return Result<void>::success();
    }
    return Result<void>::success();
  }
  OutboxRepository& repo_;
  TelegramClient* telegram_ = nullptr;
  Config cfg_;
  DeliverFn deliver_;
  std::jthread worker_;
  std::atomic<bool> running_{false};
  std::atomic<std::uint64_t> delivered_{0}, failed_{0}, dead_{0}, loops_{0};
};

} // namespace crashcore
