#pragma once
/**
 * PORT_BEHAVIOR from TestingEngine live/fencing.ts concepts
 * Generation fence: ignore late responses from superseded prediction attempts.
 */
#include "common/types.hpp"
#include <atomic>
#include <cstdint>
#include <mutex>
#include <string>
#include <unordered_map>

namespace crashcore {

class GenerationFence {
public:
  std::uint64_t bump(const std::string& key) {
    std::lock_guard lk(mu_);
    auto& g = gen_[key];
    ++g;
    return g;
  }

  std::uint64_t current(const std::string& key) const {
    std::lock_guard lk(mu_);
    auto it = gen_.find(key);
    return it == gen_.end() ? 0 : it->second;
  }

  /** True if generation is still the active one. */
  bool isCurrent(const std::string& key, std::uint64_t generation) const {
    return current(key) == generation;
  }

  void clear(const std::string& key) {
    std::lock_guard lk(mu_);
    gen_.erase(key);
  }

private:
  mutable std::mutex mu_;
  std::unordered_map<std::string, std::uint64_t> gen_;
};

/** Monotonic global fence for whole-process epoch (reconnects, restarts). */
class GlobalEpoch {
public:
  std::uint64_t bump() noexcept { return epoch_.fetch_add(1, std::memory_order_acq_rel) + 1; }
  std::uint64_t get() const noexcept { return epoch_.load(std::memory_order_acquire); }
  bool isCurrent(std::uint64_t e) const noexcept { return get() == e; }
private:
  std::atomic<std::uint64_t> epoch_{1};
};

} // namespace crashcore
