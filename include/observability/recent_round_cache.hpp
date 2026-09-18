#pragma once
/**
 * Bounded LRU hot cache keyed by gameId — TE hot-cache.ts parity.
 * Avoids DB round-trips for recently seen rounds during REST backfill / live path.
 */
#include "common/types.hpp"
#include "timing/timestamp.hpp"
#include <list>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>

namespace crashcore {

struct CachedRound {
  std::string gameId;
  double multiplier = 0.0;
  TimestampMs endedAtMs = 0;
  TimestampMs cachedAtMs = 0;
  std::string hash;
};

class RecentRoundCache {
public:
  explicit RecentRoundCache(std::size_t capacity = 200) : capacity_(capacity) {}

  void put(CachedRound r) {
    std::lock_guard lk(mu_);
    r.cachedAtMs = r.cachedAtMs ? r.cachedAtMs : nowMs();
    auto it = map_.find(r.gameId);
    if (it != map_.end()) {
      order_.splice(order_.end(), order_, it->second);
      *it->second = std::move(r);
      return;
    }
    order_.push_back(std::move(r));
    auto lit = std::prev(order_.end());
    map_[lit->gameId] = lit;
    while (map_.size() > capacity_) {
      const auto& oldest = order_.front();
      map_.erase(oldest.gameId);
      order_.pop_front();
    }
  }

  std::optional<CachedRound> get(const std::string& gameId) {
    std::lock_guard lk(mu_);
    auto it = map_.find(gameId);
    if (it == map_.end()) return std::nullopt;
    order_.splice(order_.end(), order_, it->second);
    return *it->second;
  }

  bool contains(const std::string& gameId) const {
    std::lock_guard lk(mu_);
    return map_.count(gameId) > 0;
  }

  std::size_t size() const {
    std::lock_guard lk(mu_);
    return map_.size();
  }

  void clear() {
    std::lock_guard lk(mu_);
    map_.clear();
    order_.clear();
  }

private:
  std::size_t capacity_;
  mutable std::mutex mu_;
  std::list<CachedRound> order_;
  std::unordered_map<std::string, std::list<CachedRound>::iterator> map_;
};

inline RecentRoundCache& globalRecentRoundCache() {
  static RecentRoundCache cache(200);
  return cache;
}

} // namespace crashcore
