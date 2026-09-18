#pragma once
#include "ingestion/heartbeat/heartbeat.hpp"
#include "common/constants.hpp"
#include <atomic>
#include <functional>

namespace crashcore {

class HeartbeatMonitor {
public:
  using TimeoutHandler = std::function<void()>;

  explicit HeartbeatMonitor(std::int64_t intervalMs = constants::HEARTBEAT_INTERVAL_MS,
                            std::int64_t timeoutMs = constants::HEARTBEAT_TIMEOUT_MS)
      : interval_ms_(intervalMs), timeout_ms_(timeoutMs) {}

  void onPingSent() {
    state_.lastPingMs = nowMs();
    ++state_.pingsSent;
  }

  void onPongReceived() {
    state_.lastPongMs = nowMs();
    ++state_.pongsReceived;
    state_.timedOut = false;
  }

  bool check() {
    const auto now = nowMs();
    if (state_.lastPingMs > 0 && (now - state_.lastPingMs) > timeout_ms_ &&
        state_.lastPongMs < state_.lastPingMs) {
      state_.timedOut = true;
      if (on_timeout_) on_timeout_();
      return false;
    }
    return true;
  }

  bool shouldPing() const {
    if (state_.lastPingMs == 0) return true;
    return (nowMs() - state_.lastPingMs) >= interval_ms_;
  }

  void setTimeoutHandler(TimeoutHandler h) { on_timeout_ = std::move(h); }
  const HeartbeatState& state() const noexcept { return state_; }

private:
  std::int64_t interval_ms_;
  std::int64_t timeout_ms_;
  HeartbeatState state_;
  TimeoutHandler on_timeout_;
};

} // namespace crashcore
