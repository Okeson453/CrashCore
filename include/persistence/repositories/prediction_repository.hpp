#pragma once
#include "persistence/repository.hpp"
#include "persistence/database.hpp"
#include "persistence/sql_builder.hpp"
#include "common/types.hpp"
#include <unordered_map>
#include <mutex>

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
    std::lock_guard lk(mu_);
    auto it = cache_.find(id);
    if (it != cache_.end()) return std::optional<PredictionEvent>(it->second);
    return std::optional<PredictionEvent>{};
  }

  Result<std::vector<PredictionEvent>> findRecent(std::size_t limit) override {
    std::lock_guard lk(mu_);
    std::vector<PredictionEvent> out;
    for (const auto& [_, v] : cache_) {
      out.push_back(v);
      if (out.size() >= limit) break;
    }
    return out;
  }

  Result<std::vector<PredictionEvent>> findByRound(const RoundId& roundId) {
    std::lock_guard lk(mu_);
    std::vector<PredictionEvent> out;
    for (const auto& [_, v] : cache_) {
      if (v.targetRoundId == roundId) out.push_back(v);
    }
    return out;
  }

  std::uint64_t upsertCount() const noexcept { return upserts_; }

private:
  Database& db_;
  mutable std::mutex mu_;
  std::unordered_map<PredictionId, PredictionEvent> cache_;
  std::uint64_t upserts_ = 0;
};

} // namespace crashcore
