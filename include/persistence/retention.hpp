#pragma once
/**
 * PORT_BEHAVIOR concepts from TestingEngine live/retention.ts
 * Prune policies for in-memory caches and DB (when connected).
 */
#include "common/types.hpp"
#include "timing/timestamp.hpp"
#include "persistence/database.hpp"
#include "common/result.hpp"
#include <string>

namespace crashcore {

struct RetentionPolicy {
  TimestampMs roundsMaxAgeMs = 7LL * 24 * 3600 * 1000; // 7d
  TimestampMs predictionsMaxAgeMs = 3LL * 24 * 3600 * 1000;
  TimestampMs outcomesMaxAgeMs = 7LL * 24 * 3600 * 1000;
  TimestampMs outboxDeliveredMaxAgeMs = 2LL * 24 * 3600 * 1000;
  std::size_t maxInMemoryRounds = 10000;
  std::size_t maxInMemoryPredictions = 5000;
};

class RetentionManager {
public:
  explicit RetentionManager(Database* db = nullptr, RetentionPolicy policy = {})
      : db_(db), policy_(policy) {}

  Result<void> pruneDatabase() {
    if (!db_ || !db_->connected()) return Result<void>::success();
    const auto now = nowMs();
    auto run = [&](const std::string& sql) {
      auto r = db_->execute(sql);
      if (r) ++statements_;
      return r;
    };
    // Best-effort deletes by age
    auto r1 = run("DELETE FROM crash_rounds WHERE crashed_at_ms > 0 AND crashed_at_ms < " +
                  std::to_string(now - policy_.roundsMaxAgeMs));
    if (!r1) return r1.error();
    auto r2 = run("DELETE FROM predictions WHERE created_at_ms > 0 AND created_at_ms < " +
                  std::to_string(now - policy_.predictionsMaxAgeMs));
    if (!r2) return r2.error();
    auto r3 = run("DELETE FROM outcomes WHERE resolved_at_ms > 0 AND resolved_at_ms < " +
                  std::to_string(now - policy_.outcomesMaxAgeMs));
    if (!r3) return r3.error();
    auto r4 = run("DELETE FROM notification_outbox WHERE state = 3 AND delivered_at_ms > 0 AND delivered_at_ms < " +
                  std::to_string(now - policy_.outboxDeliveredMaxAgeMs));
    if (!r4) return r4.error();
    ++runs_;
    return Result<void>::success();
  }

  const RetentionPolicy& policy() const noexcept { return policy_; }
  std::uint64_t runCount() const noexcept { return runs_; }
  std::uint64_t statementCount() const noexcept { return statements_; }

private:
  Database* db_;
  RetentionPolicy policy_;
  std::uint64_t runs_ = 0, statements_ = 0;
};

} // namespace crashcore
