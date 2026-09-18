#pragma once
/**
 * Exponential backoff reconnect policy with jitter and WAF-aware ceiling.
 * From TestingEngine native-socket-client reconnect behaviour.
 */
#include "common/constants.hpp"
#include <algorithm>
#include <cstdint>
#include <random>

namespace crashcore {

struct ReconnectPolicy {
  std::int64_t baseMs = constants::RECONNECT_BASE_MS;
  std::int64_t maxMs = constants::RECONNECT_MAX_MS;
  std::int64_t wafBackoffMs = 12'000;
  int maxAttempts = 100;
  double jitterRatio = 0.2; // ±20%

  std::int64_t delayMs(int attempt, bool wafBlocked = false) const {
    if (wafBlocked) return wafBackoffMs;
    if (attempt < 0) attempt = 0;
    if (attempt > 20) attempt = 20; // cap shift
    std::int64_t d = baseMs * (1LL << attempt);
    if (d > maxMs) d = maxMs;
    return applyJitter(d);
  }

  std::int64_t applyJitter(std::int64_t delay) const {
    if (jitterRatio <= 0.0) return delay;
    static thread_local std::mt19937_64 rng{std::random_device{}()};
    std::uniform_real_distribution<double> dist(1.0 - jitterRatio, 1.0 + jitterRatio);
    auto j = static_cast<std::int64_t>(static_cast<double>(delay) * dist(rng));
    return std::max<std::int64_t>(1, j);
  }

  bool shouldGiveUp(int attempt) const noexcept {
    return attempt >= maxAttempts;
  }
};

} // namespace crashcore
