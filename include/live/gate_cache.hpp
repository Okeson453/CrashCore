#pragma once
#include "timing/timestamp.hpp"
#include <atomic>
#include <mutex>
#include <string>
namespace crashcore {
class GateCache {
public:
  void setWorkerId(std::string id) { std::lock_guard lk(mu_); worker_id_ = std::move(id); }
  std::string workerId() const { std::lock_guard lk(mu_); return worker_id_; }
  void setEpoch(std::uint64_t e) { epoch_.store(e); }
  std::uint64_t epoch() const noexcept { return epoch_.load(); }
  void setHasAuthority(bool v) { authority_.store(v); }
  bool hasAuthority() const noexcept { return authority_.load(); }
  bool allow(std::uint64_t expectedEpoch) const {
    return authority_.load() && epoch_.load() == expectedEpoch;
  }
  void noteHeartbeat() { last_hb_ms_.store(nowMs()); }
  bool isStale(std::int64_t maxAgeMs = 15000) const {
    auto last = last_hb_ms_.load();
    return !last || (nowMs() - last) > maxAgeMs;
  }
private:
  mutable std::mutex mu_;
  std::string worker_id_;
  std::atomic<std::uint64_t> epoch_{0};
  std::atomic<bool> authority_{false};
  std::atomic<TimestampMs> last_hb_ms_{0};
};
inline GateCache& globalGateCache() { static GateCache g; return g; }
} // namespace crashcore
