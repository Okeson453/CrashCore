#pragma once
#include "persistence/database.hpp"
#include "persistence/pg/pg_database.hpp"
#include "common/result.hpp"
#include "common/errors.hpp"
#include "timing/timestamp.hpp"
#include <string>
#include <vector>
#include <atomic>
#include <cstdlib>

namespace crashcore {

struct OutboxRow {
  std::string id, dedupeKey, payload, status, lastError;
  int attempts = 0;
  TimestampMs createdAtMs = 0, availableAtMs = 0;
};

class OutboxRepository {
public:
  explicit OutboxRepository(Database& db) : db_(db) {}

  Result<void> enqueue(const OutboxRow& row) {
    return enqueueWithRound(row, "");
  }

  Result<void> enqueueWithRound(const OutboxRow& row, const std::string& roundId) {
    auto* pg = dynamic_cast<PgDatabase*>(&db_);
    if (pg) {
      // id is BIGSERIAL — omit it; store caller id in prediction_id
      auto r = pg->executeParams(
        "INSERT INTO notification_outbox (dedupe_key, payload, status, attempts, created_at_ms, available_at_ms, round_id, prediction_id) "
        "VALUES ($1,$2,'pending',0,$3,$4,$5,$6) ON CONFLICT (dedupe_key) DO NOTHING",
        {row.dedupeKey, row.payload,
         std::to_string(row.createdAtMs ? row.createdAtMs : nowMs()),
         std::to_string(row.availableAtMs ? row.availableAtMs : nowMs()),
         roundId, row.id});
      if (r) ++enqueued_;
      return r;
    }
    auto r = db_.execute(
        "INSERT INTO notification_outbox (dedupe_key, payload, status) VALUES ('" +
        row.dedupeKey + "','" + row.payload + "','pending')");
    if (r) ++enqueued_;
    return r;
  }

  Result<std::vector<OutboxRow>> claim(std::size_t limit, const std::string& workerId) {
    auto* pg = dynamic_cast<PgDatabase*>(&db_);
    if (!pg) return std::vector<OutboxRow>{};
    auto qr = pg->query(
      "UPDATE notification_outbox SET status='claimed', attempts=attempts+1, worker_id=$1, claimed_at_ms=$2 "
      "WHERE id IN (SELECT id FROM notification_outbox WHERE status='pending' AND available_at_ms <= $2 "
      "ORDER BY created_at_ms ASC FOR UPDATE SKIP LOCKED LIMIT $3) "
      "RETURNING id, dedupe_key, payload, status, attempts, created_at_ms, available_at_ms, COALESCE(last_error,'')",
      {workerId, std::to_string(nowMs()), std::to_string(limit)});
    if (!qr) return Error{qr.error().code, qr.error().message};
    std::vector<OutboxRow> rows;
    for (const auto& r : qr.value().rows) {
      if (r.size() < 8) continue;
      OutboxRow o;
      o.id=r[0]; o.dedupeKey=r[1]; o.payload=r[2]; o.status=r[3];
      o.attempts=std::atoi(r[4].c_str());
      o.createdAtMs=std::strtoll(r[5].c_str(),nullptr,10);
      o.availableAtMs=std::strtoll(r[6].c_str(),nullptr,10);
      o.lastError=r[7];
      rows.push_back(std::move(o));
    }
    claimed_ += rows.size();
    return rows;
  }

  Result<void> markDelivered(const std::string& id) {
    auto* pg = dynamic_cast<PgDatabase*>(&db_);
    if (pg) {
      auto r = pg->executeParams(
        "UPDATE notification_outbox SET status='delivered', delivered_at_ms=$1 WHERE id=$2",
        {std::to_string(nowMs()), id});
      if (r) ++delivered_;
      return r;
    }
    return db_.execute("UPDATE notification_outbox SET status='delivered' WHERE id='" + id + "'");
  }

  Result<void> markFailed(const std::string& id, const std::string& err,
                          std::int64_t retryAfterMs, bool deadLetter) {
    auto* pg = dynamic_cast<PgDatabase*>(&db_);
    const std::string status = deadLetter ? "dead" : "pending";
    if (pg) {
      auto r = pg->executeParams(
        "UPDATE notification_outbox SET status=$1, last_error=$2, available_at_ms=$3 WHERE id=$4",
        {status, err, std::to_string(nowMs() + retryAfterMs), id});
      if (r) { if (deadLetter) ++dead_; else ++failed_; }
      return r;
    }
    return Result<void>::success();
  }

  Result<void> recoverStaleClaims(std::int64_t olderThanMs) {
    auto* pg = dynamic_cast<PgDatabase*>(&db_);
    if (pg) {
      return pg->executeParams(
        "UPDATE notification_outbox SET status='pending', worker_id=NULL "
        "WHERE status='claimed' AND claimed_at_ms < $1",
        {std::to_string(nowMs() - olderThanMs)});
    }
    return Result<void>::success();
  }

  std::uint64_t enqueuedCount() const noexcept { return enqueued_.load(); }
  std::uint64_t claimedCount() const noexcept { return claimed_.load(); }
  std::uint64_t deliveredCount() const noexcept { return delivered_.load(); }

private:
  Database& db_;
  std::atomic<std::uint64_t> enqueued_{0}, claimed_{0}, delivered_{0}, failed_{0}, dead_{0};
};

} // namespace crashcore
