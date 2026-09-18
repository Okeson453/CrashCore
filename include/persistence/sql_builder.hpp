#pragma once
/**
 * Lightweight SQL fragment builders for repositories (parameter-style placeholders).
 * Actual binding done by libpq layer when connected.
 */
#include "common/types.hpp"
#include "validation/outcome.hpp"
#include <string>
#include <sstream>
#include <iomanip>

namespace crashcore {

class SqlBuilder {
public:
  static std::string upsertCrashRound(const CrashRound& r) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(4);
    oss << "INSERT INTO crash_rounds(game_id, multiplier, hash, salt, began_at_ms, crashed_at_ms, sequence) VALUES("
        << quote(r.gameId) << ", " << r.multiplier << ", "
        << quote(r.hash) << ", " << quote(r.salt) << ", "
        << r.beganAtMs << ", " << r.crashedAtMs << ", " << r.sequence
        << ") ON CONFLICT (game_id) DO UPDATE SET "
        << "multiplier = EXCLUDED.multiplier, "
        << "hash = COALESCE(EXCLUDED.hash, crash_rounds.hash), "
        << "crashed_at_ms = EXCLUDED.crashed_at_ms, "
        << "sequence = EXCLUDED.sequence";
    return oss.str();
  }

  static std::string upsertPrediction(const PredictionEvent& e) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(6);
    oss << "INSERT INTO predictions(id, round_id, game_id, decision, confidence, entry_mult, target_mult, correlation_id, model_version, created_at_ms) VALUES("
        << quote(e.predictionId) << ", " << quote(e.targetRoundId) << ", "
        << quote(e.gameId) << ", " << static_cast<int>(e.decision) << ", "
        << e.confidence << ", " << e.entryMult << ", " << e.targetMult << ", "
        << quote(e.correlationId) << ", " << quote(e.modelVersion) << ", "
        << e.createdAtMs
        << ") ON CONFLICT (id) DO NOTHING";
    return oss.str();
  }

  static std::string upsertOutcome(const Outcome& o) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(4);
    oss << "INSERT INTO outcomes(prediction_id, round_id, result, actual_mult, target_mult, is_win, resolved_at_ms) VALUES("
        << quote(o.predictionId) << ", " << quote(o.roundId) << ", "
        << quote(outcomeName(o.result)) << ", "
        << o.actualMult << ", " << o.targetMult << ", "
        << (o.isWin ? "TRUE" : "FALSE") << ", " << o.resolvedAtMs
        << ") ON CONFLICT (prediction_id) DO NOTHING";
    return oss.str();
  }

  static std::string insertOutbox(const OutboxItem& item) {
    std::ostringstream oss;
    oss << "INSERT INTO notification_outbox(state, prediction_id, round_id, signal_kind, text, dedupe_key, attempts, created_at_ms) VALUES("
        << static_cast<int>(item.state) << ", "
        << quote(item.signal.predictionId) << ", "
        << quote(item.signal.roundId) << ", "
        << static_cast<int>(item.signal.kind) << ", "
        << quote(item.signal.text) << ", "
        << quote(item.dedupeKey) << ", "
        << item.attempts << ", " << item.createdAtMs
        << ") ON CONFLICT (dedupe_key) DO NOTHING";
    return oss.str();
  }

  static std::string claimOutboxBatch(int limit = 32) {
    return "UPDATE notification_outbox SET state = 1, claimed_at_ms = "
           "(EXTRACT(EPOCH FROM NOW())*1000)::BIGINT "
           "WHERE id IN ("
           "  SELECT id FROM notification_outbox WHERE state = 0 "
           "  ORDER BY created_at_ms ASC LIMIT " + std::to_string(limit) +
           "  FOR UPDATE SKIP LOCKED"
           ") RETURNING *";
  }

  static std::string markOutboxDelivered(std::uint64_t id) {
    return "UPDATE notification_outbox SET state = 3, delivered_at_ms = "
           "(EXTRACT(EPOCH FROM NOW())*1000)::BIGINT WHERE id = " +
           std::to_string(id);
  }


  static std::string selectPredictionById(const std::string& id) {
    return "SELECT id, round_id, game_id, decision, confidence, entry_mult, target_mult, "
           "correlation_id, model_version, created_at_ms FROM predictions WHERE id = " +
           quote(id) + " LIMIT 1";
  }

  static std::string selectRecentPredictions(std::size_t limit) {
    return "SELECT id, round_id, game_id, decision, confidence, entry_mult, target_mult, "
           "correlation_id, model_version, created_at_ms FROM predictions "
           "ORDER BY created_at_ms DESC LIMIT " + std::to_string(limit);
  }

  static std::string selectPredictionsByRound(const std::string& roundId) {
    return "SELECT id, round_id, game_id, decision, confidence, entry_mult, target_mult, "
           "correlation_id, model_version, created_at_ms FROM predictions WHERE round_id = " +
           quote(roundId);
  }


  static std::string selectOutcomeById(const std::string& id) {
    return "SELECT prediction_id, round_id, result, actual_mult, target_mult, is_win, resolved_at_ms "
           "FROM outcomes WHERE prediction_id = " + quote(id) + " LIMIT 1";
  }

  static std::string selectCrashRoundById(const std::string& id) {
    return "SELECT game_id, multiplier, hash, salt, began_at_ms, crashed_at_ms, sequence "
           "FROM crash_rounds WHERE game_id = " + quote(id) + " LIMIT 1";
  }

  static std::string selectRecentCrashRounds(std::size_t limit) {
    return "SELECT game_id, multiplier, hash, salt, began_at_ms, crashed_at_ms, sequence "
           "FROM crash_rounds ORDER BY crashed_at_ms DESC LIMIT " + std::to_string(limit);
  }

private:
  static std::string quote(const std::string& s) {
    std::string out = "'";
    for (char c : s) {
      if (c == '\'') out += "''";
      else out += c;
    }
    out += "'";
    return out;
  }
};

} // namespace crashcore
