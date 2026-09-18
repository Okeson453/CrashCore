#pragma once
/**
 * Phase 1 — transactional prediction + target ownership + outbox row.
 * Mirrors TestingEngine: persist prediction, unmatched target claim, outbox
 * row atomically before delivery wake.
 */
#include "persistence/database.hpp"
#include "persistence/pg/pg_database.hpp"
#include "persistence/transaction.hpp"
#include "persistence/outbox_repository.hpp"
#include "prediction_interface/prediction_event.hpp"
#include "delivery/signal.hpp"
#include "delivery/outbox/outbox.hpp"
#include "delivery/outbox/durable_handoff.hpp"
#include "common/result.hpp"
#include "common/errors.hpp"
#include "timing/timestamp.hpp"
#include <atomic>
#include <mutex>
#include <string>

namespace crashcore {

struct DurablePersistResult {
  bool ok = false;
  bool durable = false;   // true if DB transaction committed
  bool memoryPublished = false;
  std::string error;
};

/**
 * Persist actionable prediction under a single transaction when DB is present:
 *   1) INSERT predictions
 *   2) INSERT target_claims (unmatched ownership)
 *   3) INSERT notification_outbox
 * Then publish to in-memory outbox for immediate worker wake.
 * If DB absent, falls back to memory-only handoff (non-durable mode).
 */
class DurablePredictionTx {
public:
  DurablePredictionTx(Database* db,
                      OutboxRepository* outboxRepo,
                      DurableHandoff* handoff,
                      Outbox* memoryOutbox = nullptr)
      : db_(db), repo_(outboxRepo), handoff_(handoff), memory_(memoryOutbox) {}

  DurablePersistResult persistPrediction(const PredictionEvent& pev, Signal sig) {
    DurablePersistResult out;
    const auto now = nowMs();

    // Always try memory handoff for low-latency wake
    if (handoff_) {
      auto pub = handoff_->publishPrediction(sig);
      out.memoryPublished = static_cast<bool>(pub);
    } else if (memory_) {
      auto pub = memory_->publish(sig);
      out.memoryPublished = static_cast<bool>(pub);
    }

    if (!db_ || !repo_) {
      // Non-durable mode
      out.ok = out.memoryPublished;
      out.durable = false;
      if (!out.ok) out.error = "memory outbox publish failed";
      return out;
    }

    auto* pg = dynamic_cast<PgDatabase*>(db_);
    if (!pg) {
      out.ok = out.memoryPublished;
      out.durable = false;
      out.error = "no PgDatabase for durable path";
      return out;
    }

    Transaction tx(*db_);

    // 1) predictions row
    {
      auto r = pg->executeParams(
          "INSERT INTO predictions (id, round_id, game_id, decision, confidence, "
          "probability, entry_mult, target_mult, correlation_id, model_version, created_at_ms) "
          "VALUES ($1,$2,$3,$4,$5,$6,$7,$8,$9,$10,$11) "
          "ON CONFLICT (id) DO UPDATE SET confidence=EXCLUDED.confidence, "
          "decision=EXCLUDED.decision, model_version=EXCLUDED.model_version",
          {pev.predictionId, pev.targetRoundId, pev.gameId,
           std::to_string(static_cast<int>(pev.decision)),
           std::to_string(pev.confidence),
           std::to_string(pev.probability),
           std::to_string(pev.entryMult), std::to_string(pev.targetMult),
           pev.correlationId, pev.modelVersion, std::to_string(now)});
      if (!r) {
        out.error = std::string("predictions insert: ") + r.error().message;
        return out;
      }
    }

    // 2) target ownership (unmatched uniqueness on round_id)
    {
      auto r = pg->executeParams(
          "INSERT INTO target_claims (round_id, prediction_id, claimed_at_ms, expires_at_ms) "
          "VALUES ($1,$2,$3,$4) "
          "ON CONFLICT (round_id) DO NOTHING",
          {pev.targetRoundId, pev.predictionId, std::to_string(now),
           std::to_string(now + 120000)});
      if (!r) {
        out.error = std::string("target_claims insert: ") + r.error().message;
        return out;
      }
    }

    // 3) durable outbox row (round_id enables BG stale-kill)
    {
      OutboxRow row;
      row.id = pev.predictionId + "-pred";
      row.dedupeKey = pev.predictionId + ":pred";
      row.payload = sig.text;
      row.createdAtMs = now;
      row.availableAtMs = now;
      // Encode target round into payload prefix for temporal auth workers
      row.payload = std::string("round=") + pev.targetRoundId + "\n" + sig.text;
      auto r = repo_->enqueueWithRound(row, pev.targetRoundId);
      if (!r) {
        // Fallback if enqueueWithRound not available
        r = repo_->enqueue(row);
      }
      if (!r) {
        out.error = std::string("outbox enqueue: ") + r.error().message;
        return out;
      }
    }

    auto cr = tx.commit();
    if (!cr) {
      out.error = std::string("commit: ") + cr.error().message;
      return out;
    }

    out.ok = true;
    out.durable = true;
    ++durable_ok_;
    return out;
  }

  DurablePersistResult persistOutcome(const Outcome& o, Signal sig) {
    DurablePersistResult out;
    if (handoff_) {
      auto pub = handoff_->publishOutcome(sig);
      out.memoryPublished = static_cast<bool>(pub);
    }
    if (!db_) {
      out.ok = out.memoryPublished;
      return out;
    }
    auto* pg = dynamic_cast<PgDatabase*>(db_);
    if (!pg) {
      out.ok = out.memoryPublished;
      return out;
    }
    // outcomes table from migrations/0005 (result TEXT, resolved_at_ms)
    const char* resultName =
        o.isWin || o.result == PredictionOutcome::Win ? "win" :
        (o.result == PredictionOutcome::Loss ? "loss" : "unknown");
    auto r = pg->executeParams(
        "INSERT INTO outcomes (prediction_id, round_id, result, is_win, actual_mult, target_mult, resolved_at_ms) "
        "VALUES ($1,$2,$3,$4,$5,$6,$7) ON CONFLICT (prediction_id) DO NOTHING",
        {o.predictionId, o.roundId, resultName,
         o.isWin ? "true" : "false",
         std::to_string(o.actualMult), std::to_string(o.targetMult),
         std::to_string(nowMs())});
    out.ok = static_cast<bool>(r) || out.memoryPublished;
    out.durable = static_cast<bool>(r);
    if (!r) out.error = r.error().message;
    return out;
  }

  std::uint64_t durableOkCount() const noexcept { return durable_ok_.load(); }

private:
  Database* db_ = nullptr;
  OutboxRepository* repo_ = nullptr;
  DurableHandoff* handoff_ = nullptr;
  Outbox* memory_ = nullptr;
  std::atomic<std::uint64_t> durable_ok_{0};
};

} // namespace crashcore
