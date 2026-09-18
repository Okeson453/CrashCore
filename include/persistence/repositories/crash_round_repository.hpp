#pragma once
#include <unordered_map>
#include "persistence/repository.hpp"
#include "persistence/database.hpp"
#include "persistence/sql_builder.hpp"
#include "common/types.hpp"
#include <vector>
#include <mutex>

namespace crashcore {

class CrashRoundRepository : public Repository<CrashRound, GameId> {
public:
  explicit CrashRoundRepository(Database& db) : db_(db) {}

  Result<void> upsert(const CrashRound& r) override {
    auto sql = SqlBuilder::upsertCrashRound(r);
    auto res = db_.execute(sql);
    if (res) {
      std::lock_guard lk(mu_);
      cache_[r.gameId] = r;
      ++upserts_;
    }
    return res;
  }

  Result<std::optional<CrashRound>> findById(const GameId& id) override {
    std::lock_guard lk(mu_);
    auto it = cache_.find(id);
    if (it != cache_.end()) return std::optional<CrashRound>(it->second);
    return std::optional<CrashRound>{};
  }

  Result<std::vector<CrashRound>> findRecent(std::size_t limit) override {
    std::lock_guard lk(mu_);
    std::vector<CrashRound> out;
    out.reserve(std::min(limit, cache_.size()));
    for (const auto& [_, v] : cache_) {
      out.push_back(v);
      if (out.size() >= limit) break;
    }
    return out;
  }

  Result<void> upsertFromEvent(const CrashEvent& e) {
    CrashRound r;
    r.gameId = e.gameId.empty() ? e.roundId : e.gameId;
    r.multiplier = e.crashPoint > 0 ? e.crashPoint : e.currentMult;
    r.hash = e.hash;
    r.beganAtMs = e.beganAtMs;
    r.crashedAtMs = e.endedAtMs;
    r.sequence = e.sequence;
    r.hasHash = !e.hash.empty();
    return upsert(r);
  }

  std::uint64_t upsertCount() const noexcept { return upserts_; }
  std::size_t cacheSize() const { std::lock_guard lk(mu_); return cache_.size(); }

private:
  Database& db_;
  mutable std::mutex mu_;
  std::unordered_map<GameId, CrashRound> cache_;
  std::uint64_t upserts_ = 0;
};

} // namespace crashcore
