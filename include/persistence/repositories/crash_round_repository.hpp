#pragma once
#include <unordered_map>
#include "persistence/repository.hpp"
#include "persistence/database.hpp"
#include "persistence/sql_builder.hpp"
#include "common/types.hpp"
#include <vector>
#include <mutex>
#include <cstdlib>
#include <algorithm>

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
    {
      std::lock_guard lk(mu_);
      auto it = cache_.find(id);
      if (it != cache_.end()) return std::optional<CrashRound>(it->second);
    }
    if (db_.connected()) {
      auto qr = db_.query(SqlBuilder::selectCrashRoundById(id));
      if (qr && !qr.value().rows.empty()) {
        auto r = rowToRound(qr.value().rows[0]);
        if (r) {
          std::lock_guard lk(mu_);
          cache_[id] = *r;
          return r;
        }
      }
    }
    return std::optional<CrashRound>{};
  }

  Result<std::vector<CrashRound>> findRecent(std::size_t limit) override {
    if (db_.connected()) {
      auto qr = db_.query(SqlBuilder::selectRecentCrashRounds(limit));
      if (qr) {
        std::vector<CrashRound> out;
        for (const auto& row : qr.value().rows) {
          auto r = rowToRound(row);
          if (r) {
            out.push_back(*r);
            std::lock_guard lk(mu_);
            cache_[r->gameId] = *r;
          }
        }
        if (!out.empty()) return out;
      }
    }
    std::lock_guard lk(mu_);
    std::vector<CrashRound> out;
    out.reserve(std::min(limit, cache_.size()));
    for (const auto& [_, v] : cache_) {
      out.push_back(v);
      if (out.size() >= limit) break;
    }
    return out;
  }

  std::uint64_t upsertCount() const noexcept { return upserts_; }

private:
  static std::optional<CrashRound> rowToRound(const std::vector<std::string>& row) {
    if (row.size() < 7) return std::nullopt;
    CrashRound r;
    r.gameId = row[0];
    r.multiplier = std::atof(row[1].c_str());
    r.hash = row[2];
    r.salt = row[3];
    r.beganAtMs = std::atoll(row[4].c_str());
    r.crashedAtMs = std::atoll(row[5].c_str());
    r.sequence = static_cast<SequenceNum>(std::atoll(row[6].c_str()));
    r.hasHash = !r.hash.empty();
    r.hasSalt = !r.salt.empty();
    return r;
  }

  Database& db_;
  mutable std::mutex mu_;
  std::unordered_map<GameId, CrashRound> cache_;
  std::uint64_t upserts_ = 0;
};

} // namespace crashcore
