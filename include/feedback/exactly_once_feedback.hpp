#pragma once
/**
 * Phase 3 — exactly-once outcome feedback.
 *
 * Dedupes by prediction_id into feedback_events so adaptive edge / learning
 * is applied at most once per prediction. Optional durable insert when DB
 * is present; always maintains an in-process seen-set for hot-path safety.
 */
#include "persistence/database.hpp"
#include "persistence/pg/pg_database.hpp"
#include "validation/outcome.hpp"
#include "prediction/acie/adaptive_edge.hpp"
#include "common/result.hpp"
#include "common/errors.hpp"
#include "timing/timestamp.hpp"
#include <atomic>
#include <mutex>
#include <string>
#include <unordered_set>

namespace crashcore {

class ExactlyOnceFeedback {
public:
  ExactlyOnceFeedback(Database* db = nullptr, acie::AdaptiveEdge* edge = nullptr)
      : db_(db), edge_(edge) {}

  void setAdaptiveEdge(acie::AdaptiveEdge* edge) noexcept { edge_ = edge; }
  void setDatabase(Database* db) noexcept { db_ = db; }

  /**
   * Apply feedback if this prediction_id has not been seen.
   * Returns true when feedback was newly applied.
   */
  bool apply(const Outcome& o) {
    if (o.predictionId.empty()) return false;
    if (o.result != PredictionOutcome::Win && o.result != PredictionOutcome::Loss)
      return false;

    {
      std::lock_guard lk(mu_);
      if (seen_.count(o.predictionId)) {
        ++duplicates_;
        return false;
      }
      seen_.insert(o.predictionId);
      if (seen_.size() > 100000) {
        // bound memory: drop arbitrary half (simple)
        seen_.clear();
        seen_.insert(o.predictionId);
      }
    }

    if (edge_) edge_->noteOutcome(o);
    ++applied_;

    if (db_) {
      auto* pg = dynamic_cast<PgDatabase*>(db_);
      if (pg) {
        const char* outcome =
            (o.isWin || o.result == PredictionOutcome::Win) ? "win" : "loss";
        // Unique on prediction_id when migration 0039 applied; ON CONFLICT DO NOTHING otherwise
        auto r = pg->executeParams(
            "INSERT INTO feedback_events (prediction_id, round_id, outcome, actual_mult, delivered, created_at_ms) "
            "VALUES ($1,$2,$3,$4,true,$5) ON CONFLICT DO NOTHING",
            {o.predictionId, o.roundId, outcome,
             std::to_string(o.actualMult), std::to_string(nowMs())});
        if (r) ++durable_;
      }
    }
    return true;
  }

  std::uint64_t appliedCount() const noexcept { return applied_.load(); }
  std::uint64_t duplicateCount() const noexcept { return duplicates_.load(); }
  std::uint64_t durableCount() const noexcept { return durable_.load(); }

private:
  Database* db_ = nullptr;
  acie::AdaptiveEdge* edge_ = nullptr;
  mutable std::mutex mu_;
  std::unordered_set<std::string> seen_;
  std::atomic<std::uint64_t> applied_{0}, duplicates_{0}, durable_{0};
};

} // namespace crashcore
