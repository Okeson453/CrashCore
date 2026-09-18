#pragma once
#include "persistence/database.hpp"
#include "persistence/repositories/crash_round_repository.hpp"
#include "ingestion/rest/crash_history_client.hpp"
#include "observability/recent_round_cache.hpp"
#include "prediction_interface/rolling_history.hpp"
#include "common/result.hpp"
#include "timing/timestamp.hpp"
#include <atomic>

namespace crashcore {

class ColdStartSeeder {
public:
  struct Config {
    std::size_t minHistory = 50;
    std::size_t targetHistory = 200;
    int restPages = 5;
  };
  ColdStartSeeder(Database* db = nullptr, CrashRoundRepository* rounds = nullptr,
                  CrashHistoryClient* history = nullptr, RollingHistoryBuffer* rolling = nullptr)
      : db_(db), rounds_(rounds), history_(history), rolling_(rolling) {}
  ColdStartSeeder(Database* db, CrashRoundRepository* rounds, CrashHistoryClient* history,
                  RollingHistoryBuffer* rolling, Config cfg)
      : db_(db), rounds_(rounds), history_(history), rolling_(rolling), cfg_(cfg) {}

  Result<void> seed() {
    std::size_t loaded = 0;
    if (rounds_) {
      auto recent = rounds_->findRecent(cfg_.targetHistory);
      if (recent) {
        for (const auto& r : recent.value()) {
          globalRecentRoundCache().put(CachedRound{.gameId=r.gameId,.multiplier=r.multiplier,.endedAtMs=r.crashedAtMs,.hash=r.hash});
          if (rolling_) rolling_->pushFromEvent([&]{ CrashEvent e; e.gameId=r.gameId; e.crashPoint=r.multiplier; e.endedAtMs=r.crashedAtMs; e.kind=EventKind::End; e.valid=true; return e; }());
          ++loaded;
        }
      }
    }
    if (loaded < cfg_.minHistory && history_) {
      for (int page = 1; page <= cfg_.restPages; ++page) {
        auto pageResult = history_->fetchPage(page);
        if (!pageResult) continue;
        for (const auto& fr : pageResult.value()) {
          globalRecentRoundCache().put(CachedRound{.gameId=fr.gameId,.multiplier=fr.multiplier,.endedAtMs=fr.crashedAtMs,.hash=fr.hash});
          if (rounds_) {
            CrashRound r; r.gameId=fr.gameId; r.multiplier=fr.multiplier; r.hash=fr.hash;
            r.beganAtMs=fr.beganAtMs; r.crashedAtMs=fr.crashedAtMs;
            rounds_->upsert(r);
          }
          ++loaded;
        }
        if (loaded >= cfg_.targetHistory) break;
      }
    }
    seeded_ = loaded;
    return Result<void>::success();
  }
  std::size_t seededCount() const noexcept { return seeded_.load(); }
private:
  Database* db_ = nullptr;
  CrashRoundRepository* rounds_ = nullptr;
  CrashHistoryClient* history_ = nullptr;
  RollingHistoryBuffer* rolling_ = nullptr;
  Config cfg_;
  std::atomic<std::size_t> seeded_{0};
};

} // namespace crashcore
