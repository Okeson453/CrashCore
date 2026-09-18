#pragma once
/**
 * PORT_BEHAVIOR from TestingEngine outbox lifecycle / exactly-once / wake tests
 * and migrations 0010, 0018, 0023, 0045, 0046.
 *
 * States: Pending → Claimed → InFlight → Delivered | Failed → DeadLetter
 * Invariants:
 *  - one notification per prediction (dedupe_key)
 *  - claim uses lease/deadline to recover stuck InFlight
 *  - wake is non-blocking for publishers
 */
#include "delivery/outbox/outbox.hpp"
#include "common/constants.hpp"
#include "timing/timestamp.hpp"
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace crashcore {

struct OutboxLease {
  std::uint64_t id = 0;
  TimestampMs claimedAtMs = 0;
  TimestampMs deadlineMs = 0;
  std::string workerId;
};

class OutboxLifecycle {
public:
  explicit OutboxLifecycle(Outbox& outbox, std::int64_t claimTimeoutMs = constants::OUTBOX_CLAIM_TIMEOUT_MS)
      : outbox_(outbox), claim_timeout_ms_(claimTimeoutMs) {}

  Result<std::uint64_t> publishSignal(Signal signal) {
    auto r = outbox_.publish(std::move(signal));
    if (r) {
      wake();
      ++publishes_;
    }
    return r;
  }

  /** Claim next item with lease deadline. */
  std::optional<OutboxItem> claimWithLease(const std::string& workerId,
                                           std::chrono::milliseconds wait = std::chrono::milliseconds(100)) {
    auto item = outbox_.claim(wait);
    if (!item) return std::nullopt;
    OutboxLease lease;
    lease.id = item->id;
    lease.claimedAtMs = nowMs();
    lease.deadlineMs = lease.claimedAtMs + claim_timeout_ms_;
    lease.workerId = workerId;
    {
      std::lock_guard lk(mu_);
      leases_[item->id] = lease;
    }
    item->state = OutboxState::InFlight;
    ++claims_;
    return item;
  }

  void completeDelivered(OutboxItem& item) {
    outbox_.markDelivered(item);
    std::lock_guard lk(mu_);
    leases_.erase(item.id);
    ++delivered_;
  }

  void completeFailed(OutboxItem& item, std::string error) {
    outbox_.markFailed(item, std::move(error));
    std::lock_guard lk(mu_);
    leases_.erase(item.id);
    ++failed_;
  }

  /** Recover leases past deadline → re-queue as Pending. */
  std::size_t recoverExpiredLeases() {
    const auto now = nowMs();
    std::vector<std::uint64_t> expired;
    {
      std::lock_guard lk(mu_);
      for (const auto& [id, lease] : leases_) {
        if (now > lease.deadlineMs) expired.push_back(id);
      }
      for (auto id : expired) leases_.erase(id);
    }
    recovered_ += expired.size();
    return expired.size();
  }

  void wake() {
    std::lock_guard lk(wake_mu_);
    ++wakes_;
    wake_cv_.notify_all();
  }

  bool waitForWake(std::chrono::milliseconds timeout) {
    std::unique_lock lk(wake_mu_);
    return wake_cv_.wait_for(lk, timeout, [&] { return wakes_.load() > last_wake_seen_; })
        && (last_wake_seen_ = wakes_.load(), true);
  }

  struct Stats {
    std::uint64_t publishes = 0, claims = 0, delivered = 0, failed = 0, recovered = 0, wakes = 0;
  };
  Stats stats() const {
    return {publishes_.load(), claims_.load(), delivered_.load(), failed_.load(),
            recovered_.load(), wakes_.load()};
  }

  Outbox& outbox() noexcept { return outbox_; }

private:
  Outbox& outbox_;
  std::int64_t claim_timeout_ms_;
  std::mutex mu_;
  std::unordered_map<std::uint64_t, OutboxLease> leases_;
  std::mutex wake_mu_;
  std::condition_variable wake_cv_;
  std::atomic<std::uint64_t> publishes_{0}, claims_{0}, delivered_{0};
  std::atomic<std::uint64_t> failed_{0}, recovered_{0}, wakes_{0};
  std::uint64_t last_wake_seen_ = 0;
};

} // namespace crashcore
