#pragma once
#include "persistence/database.hpp"
#include "persistence/pg/pg_database.hpp"
#include "common/result.hpp"
#include "common/errors.hpp"
#include "timing/timestamp.hpp"
#include <atomic>
#include <chrono>
#include <functional>
#include <mutex>
#include <string>
#include <thread>
#include <unistd.h>
#include <sstream>

namespace crashcore {

class LiveSupervisor {
public:
  struct Config {
    std::string workerId;  // empty → auto unique id
    std::string lockKey = "prediction_worker";
    std::int64_t leaseTtlSeconds = 8;
    std::int64_t renewIntervalMs = 3000;
  };
  using AuthorityHandler = std::function<void(bool)>;

  LiveSupervisor(Database* db = nullptr) : db_(db) {
    ensureWorkerId();
  }
  LiveSupervisor(Database* db, Config cfg) : db_(db), cfg_(std::move(cfg)) {
    ensureWorkerId();
  }

  void setAuthorityHandler(AuthorityHandler h) { on_authority_ = std::move(h); }

  Result<void> acquire() {
    std::lock_guard lk(mu_);
    if (!db_) {
      has_authority_.store(true);
      epoch_.fetch_add(1);
      if (on_authority_) on_authority_(true);
      return Result<void>::success();
    }
    auto* pg = dynamic_cast<PgDatabase*>(db_);
    const auto now = nowMs();
    const auto expires = now + cfg_.leaseTtlSeconds * 1000;
    if (pg) {
      auto r = pg->executeParams(
        "INSERT INTO worker_leases (lock_key, owner_id, epoch, expires_at_ms) VALUES ($1,$2,1,$3) "
        "ON CONFLICT (lock_key) DO UPDATE SET owner_id=EXCLUDED.owner_id, epoch=worker_leases.epoch+1, "
        "expires_at_ms=EXCLUDED.expires_at_ms WHERE worker_leases.expires_at_ms < $4 OR worker_leases.owner_id=$2",
        {cfg_.lockKey, cfg_.workerId, std::to_string(expires), std::to_string(now)});
      if (!r) { has_authority_.store(false); return Error{r.error().code, r.error().message}; }
      // Read back epoch for fencing
      auto qr = pg->query(
        "SELECT epoch FROM worker_leases WHERE lock_key=$1 AND owner_id=$2",
        {cfg_.lockKey, cfg_.workerId});
      if (qr && !qr.value().rows.empty() && !qr.value().rows[0].empty()) {
        epoch_.store(static_cast<std::uint64_t>(std::atoll(qr.value().rows[0][0].c_str())));
      } else {
        epoch_.fetch_add(1);
      }
    } else {
      epoch_.fetch_add(1);
    }
    has_authority_.store(true);
    if (on_authority_) on_authority_(true);
    return Result<void>::success();
  }

  Result<void> renew() {
    if (!has_authority_.load()) return Error{ErrorCode::AuthFailed, "no authority"};
    if (!db_) return Result<void>::success();
    auto* pg = dynamic_cast<PgDatabase*>(db_);
    if (pg) {
      // Fence: renew only if we still own the lease at the current epoch.
      auto r = pg->executeParams(
        "UPDATE worker_leases SET expires_at_ms=$1 WHERE lock_key=$2 AND owner_id=$3 AND epoch=$4",
        {std::to_string(nowMs() + cfg_.leaseTtlSeconds * 1000), cfg_.lockKey, cfg_.workerId,
         std::to_string(epoch_.load())});
      if (!r) { loseAuthority(); return Error{r.error().code, r.error().message}; }
      // Verify ownership still holds
      auto qr = pg->query(
        "SELECT owner_id, epoch FROM worker_leases WHERE lock_key=$1",
        {cfg_.lockKey});
      if (!qr || qr.value().rows.empty() ||
          qr.value().rows[0][0] != cfg_.workerId) {
        loseAuthority();
        return Error{ErrorCode::AuthFailed, "lease ownership lost"};
      }
    }
    ++renewals_;
    return Result<void>::success();
  }

  void release() {
    if (db_) {
      auto* pg = dynamic_cast<PgDatabase*>(db_);
      if (pg) pg->executeParams(
        "DELETE FROM worker_leases WHERE lock_key=$1 AND owner_id=$2 AND epoch=$3",
        {cfg_.lockKey, cfg_.workerId, std::to_string(epoch_.load())});
    }
    loseAuthority();
  }

  void startRenewLoop() {
    if (renew_running_.exchange(true)) return;
    renew_thread_ = std::jthread([this](std::stop_token st) {
      while (!st.stop_requested() && renew_running_.load()) {
        if (has_authority_.load()) renew();
        for (std::int64_t w = 0; w < cfg_.renewIntervalMs && !st.stop_requested(); w += 50)
          std::this_thread::sleep_for(std::chrono::milliseconds(50));
      }
    });
  }
  void stopRenewLoop() {
    renew_running_.store(false);
    if (renew_thread_.joinable()) { renew_thread_.request_stop(); renew_thread_.join(); }
  }

  bool hasAuthority() const noexcept { return has_authority_.load(); }
  std::uint64_t epoch() const noexcept { return epoch_.load(); }
  const std::string& workerId() const noexcept { return cfg_.workerId; }

  /** Guard application mutations: return false if epoch no longer matches. */
  bool checkEpoch(std::uint64_t expected) const noexcept {
    return has_authority_.load() && epoch_.load() == expected;
  }

private:
  void ensureWorkerId() {
    if (!cfg_.workerId.empty()) return;
    std::ostringstream oss;
    oss << "worker-" << static_cast<long long>(::getpid()) << "-" << nowMs();
    cfg_.workerId = oss.str();
  }
  void loseAuthority() {
    if (has_authority_.exchange(false) && on_authority_) on_authority_(false);
  }
  Database* db_ = nullptr;
  Config cfg_;
  AuthorityHandler on_authority_;
  std::mutex mu_;
  std::atomic<bool> has_authority_{false};
  std::atomic<std::uint64_t> epoch_{0}, renewals_{0};
  std::atomic<bool> renew_running_{false};
  std::jthread renew_thread_;
};

} // namespace crashcore
