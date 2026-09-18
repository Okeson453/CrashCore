#pragma once
#include "persistence/repository.hpp"
#include "persistence/database.hpp"
#include "persistence/sql_builder.hpp"
#include "common/types.hpp"
#include <unordered_map>
#include <mutex>
#include <cstdlib>

namespace crashcore {

class PredictionRepository : public Repository<PredictionEvent, PredictionId> {
public:
  explicit PredictionRepository(Database& db) : db_(db) {}

  Result<void> upsert(const PredictionEvent& e) override {
    auto sql = SqlBuilder::upsertPrediction(e);
    auto res = db_.execute(sql);
    if (res) {
      std::lock_guard lk(mu_);
      cache_[e.predictionId] = e;
      ++upserts_;
    }
    return res;
  }

  Result<std::optional<PredictionEvent>> findById(const PredictionId& id) override {
    {
      std::lock_guard lk(mu_);
      auto it = cache_.find(id);
      if (it != cache_.end()) return std::optional<PredictionEvent>(it->second);
    }
    // Durable path: query Postgres on cache miss (survives restart).
    if (db_.connected()) {
      auto qr = db_.query(SqlBuilder::selectPredictionById(id));
      if (qr && !qr.value().rows.empty()) {
        auto ev = rowToEvent(qr.value().rows[0], qr.value().columns);
        if (ev) {
          std::lock_guard lk(mu_);
          cache_[id] = *ev;
          return ev;
        }
      }
    }
    return std::optional<PredictionEvent>{};
  }

  Result<std::vector<PredictionEvent>> findRecent(std::size_t limit) override {
    if (db_.connected()) {
      auto qr = db_.query(SqlBuilder::selectRecentPredictions(limit));
      if (qr) {
        std::vector<PredictionEvent> out;
        out.reserve(qr.value().rows.size());
        for (const auto& row : qr.value().rows) {
          auto ev = rowToEvent(row, qr.value().columns);
          if (ev) {
            out.push_back(*ev);
            std::lock_guard lk(mu_);
            cache_[ev->predictionId] = *ev;
          }
        }
        if (!out.empty()) return out;
      }
    }
    // Fallback to in-memory cache (tests / offline)
    std::lock_guard lk(mu_);
    std::vector<PredictionEvent> out;
    for (const auto& [_, v] : cache_) {
      out.push_back(v);
      if (out.size() >= limit) break;
    }
    return out;
  }

  Result<std::vector<PredictionEvent>> findByRound(const RoundId& roundId) {
    if (db_.connected()) {
      auto qr = db_.query(SqlBuilder::selectPredictionsByRound(roundId));
      if (qr) {
        std::vector<PredictionEvent> out;
        for (const auto& row : qr.value().rows) {
          auto ev = rowToEvent(row, qr.value().columns);
          if (ev) {
            out.push_back(*ev);
            std::lock_guard lk(mu_);
            cache_[ev->predictionId] = *ev;
          }
        }
        return out;
      }
    }
    std::lock_guard lk(mu_);
    std::vector<PredictionEvent> out;
    for (const auto& [_, v] : cache_) {
      if (v.targetRoundId == roundId) out.push_back(v);
    }
    return out;
  }

  std::uint64_t upsertCount() const noexcept { return upserts_; }

private:
  static std::optional<PredictionEvent> rowToEvent(
      const std::vector<std::string>& row,
      const std::vector<std::string>& cols) {
    if (row.size() < 10) return std::nullopt;
    // Expected order from SELECT: id, round_id, game_id, decision, confidence,
    // entry_mult, target_mult, correlation_id, model_version, created_at_ms
    PredictionEvent e;
    e.predictionId = row[0];
    e.targetRoundId = row[1];
    e.gameId = row[2];
    e.decision = static_cast<PredictionDecision>(std::atoi(row[3].c_str()));
    e.confidence = std::atof(row[4].c_str());
    e.entryMult = std::atof(row[5].c_str());
    e.targetMult = std::atof(row[6].c_str());
    e.correlationId = row[7];
    e.modelVersion = row[8];
    e.createdAtMs = std::atoll(row[9].c_str());
    return e;
  }

  Database& db_;
  mutable std::mutex mu_;
  std::unordered_map<PredictionId, PredictionEvent> cache_;
  std::uint64_t upserts_ = 0;
};

} // namespace crashcore
