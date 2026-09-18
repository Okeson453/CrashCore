#pragma once
#include "ingestion/rest/crash_history_client.hpp"
#include "ingestion/event_router.hpp"
#include "ingestion/crash_event.hpp"
#include "timing/timestamp.hpp"
#include <atomic>
#include <chrono>
#include <functional>
#include <memory>
#include <random>
#include <thread>

namespace crashcore {

class PollWorker {
public:
  struct Config {
    std::int64_t intervalMs = 5000;
    std::int64_t minBackoffMs = 1000;
    std::int64_t maxBackoffMs = 30000;
    int coldMaxPages = 20;
    int warmMaxPages = 2;
    int pageSize = 50;
    bool parallelFirstTwo = true;
  };
  using RoundHandler = std::function<void(const FetchedRound&)>;

  PollWorker(std::shared_ptr<CrashHistoryClient> client, EventRouter* router = nullptr)
      : client_(std::move(client)), router_(router) {}
  PollWorker(std::shared_ptr<CrashHistoryClient> client, EventRouter* router, Config cfg)
      : client_(std::move(client)), router_(router), cfg_(cfg) {}

  void setHandler(RoundHandler h) { handler_ = std::move(h); }
  void start() {
    if (running_.exchange(true)) return;
    cold_ = true;
    worker_ = std::jthread([this](std::stop_token st) { loop(st); });
  }
  void stop() {
    running_.store(false);
    if (worker_.joinable()) { worker_.request_stop(); worker_.join(); }
  }
  bool running() const noexcept { return running_.load(); }
  std::uint64_t pagesFetched() const noexcept { return pages_.load(); }
  std::uint64_t roundsIngested() const noexcept { return rounds_.load(); }
  std::uint64_t errors() const noexcept { return errors_.load(); }
  std::size_t tickOnce() { return fetchAndIngest(cold_ ? cfg_.coldMaxPages : cfg_.warmMaxPages); }

private:
  void loop(std::stop_token st) {
    std::mt19937 rng{std::random_device{}()};
    std::uniform_int_distribution<std::int64_t> jitter(0, 500);
    while (!st.stop_requested() && running_.load()) {
      const int maxPages = cold_ ? cfg_.coldMaxPages : cfg_.warmMaxPages;
      if (fetchAndIngest(maxPages) > 0) cold_ = false;
      std::int64_t sleepMs = cfg_.intervalMs + jitter(rng);
      if (errors_.load() > 0) {
        const auto fs = consecutive_fails_.load();
        sleepMs = std::min<std::int64_t>(cfg_.maxBackoffMs,
            cfg_.minBackoffMs * (std::int64_t{1} << std::min<std::uint64_t>(fs, 5)));
        sleepMs += jitter(rng);
      }
      for (std::int64_t w = 0; w < sleepMs && !st.stop_requested(); w += 50)
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
  }
  std::size_t fetchAndIngest(int maxPages) {
    std::size_t total = 0;
    std::vector<FetchedRound> batch;
    auto pull = [&](int page) {
      auto r = client_->fetchPage(page, cfg_.pageSize);
      ++pages_;
      if (!r) { ++errors_; ++consecutive_fails_; return; }
      consecutive_fails_.store(0);
      for (auto& fr : r.value()) batch.push_back(std::move(fr));
    };
    for (int p = 1; p <= maxPages; ++p) {
      if (!running_.load()) break;
      const auto before = batch.size();
      pull(p);
      if (batch.size() == before && p > 1) break;
    }
    for (const auto& fr : batch) {
      if (handler_) handler_(fr);
      if (router_) {
        CrashEvent ev;
        ev.kind = EventKind::End; ev.gameId = fr.gameId; ev.roundId = fr.gameId;
        ev.crashPoint = fr.multiplier; ev.hash = fr.hash;
        ev.beganAtMs = fr.beganAtMs; ev.endedAtMs = fr.crashedAtMs;
        ev.eventTimeMs = fr.crashedAtMs ? fr.crashedAtMs : nowMs();
        ev.valid = true;
        router_->route(ev);
      }
      ++rounds_; ++total;
    }
    return total;
  }
  std::shared_ptr<CrashHistoryClient> client_;
  EventRouter* router_ = nullptr;
  Config cfg_;
  RoundHandler handler_;
  std::jthread worker_;
  std::atomic<bool> running_{false};
  bool cold_ = true;
  std::atomic<std::uint64_t> pages_{0}, rounds_{0}, errors_{0}, consecutive_fails_{0};
};

} // namespace crashcore
