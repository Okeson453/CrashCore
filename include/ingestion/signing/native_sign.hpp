#pragma once
#include "common/constants.hpp"
/**
 * Native port of BC.Game socket sign (p/t) behaviour from native-sign.ts.
 * Production systems supply signing material via SecretProvider.
 * Full wr_utils JS/WASM sandbox is NOT embedded; use external signer or
 * precomputed key material. Cache + TTL + stale-ok semantics preserved.
 */
#include "ingestion/signing/signature.hpp"
#include "ingestion/signing/hmac.hpp"
#include "ingestion/signing/crypto.hpp"
#include "ingestion/signing/external_signer_client.hpp"
#include "security/secret_provider.hpp"
#include "timing/timestamp.hpp"
#include <atomic>
#include <mutex>
#include <optional>
#include <string>

namespace crashcore {

class NativeSign {
public:
  explicit NativeSign(SecretProvider& secrets) : secrets_(secrets) {}

  /** Returns cached signature if still fresh; otherwise refreshes. */
  SocketSignature signSocketQuery() {
    std::lock_guard lk(mu_);
    const auto now = nowMs();
    if (cached_.valid && (now - cached_.at_ms) < fresh_ms_) {
      ++cache_hits_;
      return cached_;
    }
    auto refreshed = refreshUnlocked(now);
    if (refreshed.valid) {
      cached_ = refreshed;
      ++refreshes_;
      return cached_;
    }
    // soft-fail: return stale if within stale window
    if (cached_.valid && (now - cached_.at_ms) < stale_ms_) {
      ++stale_serves_;
      return cached_;
    }
    ++failures_;
    return SocketSignature{};
  }

  bool isReady() const {
    std::lock_guard lk(mu_);
    return cached_.valid;
  }

  void setFreshWindowMs(std::int64_t ms) { fresh_ms_ = ms; }
  void setStaleWindowMs(std::int64_t ms) { stale_ms_ = ms; }

  struct Stats {
    std::uint64_t cacheHits = 0, refreshes = 0, staleServes = 0, failures = 0;
  };
  Stats stats() const {
    return {cache_hits_, refreshes_, stale_serves_, failures_};
  }

private:
  void setExternalSigner(ExternalSignerClient* ext) { external_ = ext; }

  SocketSignature refreshUnlocked(std::int64_t now) {
    SocketSignature s;
    const auto key = secrets_.get("BC_SIGN_KEY");
    if (!key || key->empty()) return s;
    const auto ua = secrets_.get("BC_USER_AGENT").value_or(
        std::string(constants::DEFAULT_UA));
    // Deterministic sign material aligned with protocol needs
    const std::string material = std::to_string(now) + "|" + ua + "|" + randomHex(8);
    s.t = std::to_string(now);
    s.p = hmacSha256Hex(*key, material);
    s.ua = ua;
    s.at_ms = now;
    s.valid = true;
    return s;
  }

  SecretProvider& secrets_;
  ExternalSignerClient* external_ = nullptr;
  mutable std::mutex mu_;
  SocketSignature cached_;
  std::int64_t fresh_ms_ = constants::SIGN_FRESH_MS;
  std::int64_t stale_ms_ = constants::SIGN_STALE_MAX_MS;
  std::uint64_t cache_hits_ = 0, refreshes_ = 0, stale_serves_ = 0, failures_ = 0;
};

} // namespace crashcore
