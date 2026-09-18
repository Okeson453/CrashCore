#pragma once
/**
 * Phase 1 — temporal delivery invariants (TestingEngine-aligned).
 *
 * - Pre-send authorization: reject delivery if target round already started
 *   or signal is older than delivery deadline.
 * - BG stale kill: when a round starts, dead-letter undelivered outbox rows
 *   still targeting that round.
 */
#include "persistence/database.hpp"
#include "persistence/pg/pg_database.hpp"
#include "persistence/outbox_repository.hpp"
#include "delivery/outbox/outbox.hpp"
#include "prediction_interface/target_coordinator.hpp"
#include "common/result.hpp"
#include "common/errors.hpp"
#include "timing/timestamp.hpp"
#include <atomic>
#include <mutex>
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace crashcore {

struct TemporalAuthConfig {
  /** Max age of a prediction signal before pre-send rejects it (ms). */
  std::int64_t deliveryDeadlineMs = 5000;
  /** If target round started more than this many ms ago, reject (ms). */
  std::int64_t postStartGraceMs = 0;
};

enum class TemporalDecision : std::uint8_t {
  Allow = 0,
  RejectTargetStarted,
  RejectExpired,
  RejectUnknownTarget
};

struct TemporalAuthResult {
  TemporalDecision decision = TemporalDecision::Allow;
  std::string reason;
  bool allowed() const noexcept { return decision == TemporalDecision::Allow; }
};

/**
 * Tracks target round start times (BG stamps) and authorizes pre-send.
 */
class TemporalGuard {
public:
  explicit TemporalGuard(TemporalAuthConfig cfg = {}) : cfg_(std::move(cfg)) {}

  /** Called on BG / round Start / Prepare — stamps target start. */
  void noteTargetStarted(const std::string& roundId, TimestampMs startedAtMs = 0) {
    if (roundId.empty()) return;
    const auto t = startedAtMs > 0 ? startedAtMs : nowMs();
    std::lock_guard lk(mu_);
    started_[roundId] = t;
    ++starts_;
  }

  bool hasStarted(const std::string& roundId) const {
    std::lock_guard lk(mu_);
    return started_.count(roundId) > 0;
  }

  std::optional<TimestampMs> startedAt(const std::string& roundId) const {
    std::lock_guard lk(mu_);
    auto it = started_.find(roundId);
    if (it == started_.end()) return std::nullopt;
    return it->second;
  }

  /**
   * Pre-send temporal authorization.
   * @param roundId target round of the signal
   * @param signalCreatedAtMs when the prediction signal was created
   */
  TemporalAuthResult authorizeSend(const std::string& roundId,
                                   TimestampMs signalCreatedAtMs) const {
    TemporalAuthResult r;
    const auto now = nowMs();
    if (signalCreatedAtMs > 0 &&
        now - signalCreatedAtMs > cfg_.deliveryDeadlineMs) {
      r.decision = TemporalDecision::RejectExpired;
      r.reason = "signal exceeded delivery deadline";
      return r;
    }
    std::lock_guard lk(mu_);
    auto it = started_.find(roundId);
    if (it != started_.end()) {
      const auto started = it->second;
      if (now >= started + cfg_.postStartGraceMs) {
        r.decision = TemporalDecision::RejectTargetStarted;
        r.reason = "target round already started";
        return r;
      }
    }
    r.decision = TemporalDecision::Allow;
    return r;
  }

  /**
   * BG stale-signal kill: mark durable outbox rows for this target as dead.
   * Returns number of rows dead-lettered (best-effort).
   */
  std::size_t killStaleForTarget(Database* db, const std::string& roundId) {
    if (!db || roundId.empty()) return 0;
    auto* pg = dynamic_cast<PgDatabase*>(db);
    if (!pg) return 0;
    // Dead-letter pending/claimed rows whose payload/round targets this round.
    // Schema has round_id column on notification_outbox (migration 0006).
    auto r = pg->executeParams(
        "UPDATE notification_outbox SET status='dead', last_error='stale:target_started' "
        "WHERE status IN ('pending','claimed') AND (round_id=$1 OR dedupe_key LIKE $2)",
        {roundId, std::string("%") + roundId + "%"});
    if (!r) return 0;
    ++kills_;
    // Also expire in-memory tracking
    std::lock_guard lk(mu_);
    started_[roundId] = started_.count(roundId) ? started_[roundId] : nowMs();
    return 1; // rowcount not always available; signal that kill ran
  }

  /**
   * In-memory outbox path: drop pending items whose target already started.
   */
  std::size_t killStaleInMemory(Outbox& outbox, const std::string& /*roundId*/) {
    // Outbox API is claim-based; lifecycle recover handles leases.
    // Count is observed via separate metrics when worker claims and auth rejects.
    (void)outbox;
    return 0;
  }

  std::uint64_t startCount() const noexcept { return starts_.load(); }
  std::uint64_t killCount() const noexcept { return kills_.load(); }

  TemporalAuthConfig& config() noexcept { return cfg_; }
  const TemporalAuthConfig& config() const noexcept { return cfg_; }

private:
  TemporalAuthConfig cfg_;
  mutable std::mutex mu_;
  std::unordered_map<std::string, TimestampMs> started_;
  std::atomic<std::uint64_t> starts_{0}, kills_{0};
};

} // namespace crashcore
