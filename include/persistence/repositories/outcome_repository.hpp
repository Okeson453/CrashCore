#pragma once
#include "persistence/repository.hpp"
#include "persistence/database.hpp"
#include "persistence/sql_builder.hpp"
#include "common/types.hpp"
#include "validation/outcome.hpp"
#include <unordered_map>
#include <mutex>
#include <cstdlib>
#include <cstring>

namespace crashcore {

class OutcomeRepository : public Repository<Outcome, PredictionId> {
public:
  explicit OutcomeRepository(Database& db) : db_(db) {}

  Result<void> upsert(const Outcome& o) override {
    auto sql = SqlBuilder::upsertOutcome(o);
    auto res = db_.execute(sql);
    if (res) {
      std::lock_guard lk(mu_);
      cache_[o.predictionId] = o;
      ++upserts_;
      if (o.isWin) ++wins_; else if (o.result == PredictionOutcome::Loss) ++losses_;
    }
    return res;
  }

  Result<std::optional<Outcome>> findById(const PredictionId& id) override {
    {
      std::lock_guard lk(mu_);
      auto it = cache_.find(id);
      if (it != cache_.end()) return std::optional<Outcome>(it->second);
    }
    if (db_.connected()) {
      auto qr = db_.query(SqlBuilder::selectOutcomeById(id));
      if (qr && !qr.value().rows.empty()) {
        auto o = rowToOutcome(qr.value().rows[0]);
        if (o) {
          std::lock_guard lk(mu_);
          cache_[id] = *o;
          return o;
        }
      }
    }
    return std::optional<Outcome>{};
  }

  Result<std::vector<Outcome>> findRecent(std::size_t limit) override {
    std::lock_guard lk(mu_);
    std::vector<Outcome> out;
    for (const auto& [_, v] : cache_) {
      out.push_back(v);
      if (out.size() >= limit) break;
    }
    return out;
  }

  std::uint64_t upsertCount() const noexcept { return upserts_; }
  std::uint64_t winCount() const noexcept { return wins_; }
  std::uint64_t lossCount() const noexcept { return losses_; }

private:
  static std::optional<Outcome> rowToOutcome(const std::vector<std::string>& row) {
    if (row.size() < 7) return std::nullopt;
    Outcome o;
    o.predictionId = row[0];
    o.roundId = row[1];
    // result stored as name string in SQL
    if (row[2] == "Win") o.result = PredictionOutcome::Win;
    else if (row[2] == "Loss") o.result = PredictionOutcome::Loss;
    else if (row[2] == "Void") o.result = PredictionOutcome::Void;
    else if (row[2] == "Stale") o.result = PredictionOutcome::Stale;
    else if (row[2] == "Duplicate") o.result = PredictionOutcome::Duplicate;
    else o.result = PredictionOutcome::Pending;
    o.actualMult = std::atof(row[3].c_str());
    o.targetMult = std::atof(row[4].c_str());
    o.isWin = (row[5] == "t" || row[5] == "true" || row[5] == "TRUE" || row[5] == "1");
    o.resolvedAtMs = std::atoll(row[6].c_str());
    return o;
  }

  Database& db_;
  mutable std::mutex mu_;
  std::unordered_map<PredictionId, Outcome> cache_;
  std::uint64_t upserts_ = 0, wins_ = 0, losses_ = 0;
};

} // namespace crashcore
