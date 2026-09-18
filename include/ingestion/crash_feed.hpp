#pragma once
/**
 * High-level crash feed orchestrating sign → connect → decode → route.
 * Full WebSocket/Beast wiring is the production next step; this provides
 * the control plane and replay/inject path used by tests and diagnostics.
 */
#include "ingestion/event_decoder.hpp"
#include "ingestion/event_router.hpp"
#include "ingestion/signing/native_sign.hpp"
#include "ingestion/heartbeat/heartbeat_monitor.hpp"
#include "timing/latency_tracker.hpp"
#include "common/result.hpp"
#include "common/types.hpp"
#include <atomic>
#include <functional>
#include <string>

namespace crashcore {

enum class FeedState : std::uint8_t {
  Stopped = 0,
  Connecting,
  Connected,
  Degraded,
  Reconnecting
};

class CrashFeed {
public:
  using FrameHandler = std::function<void(const CrashEvent&)>;

  CrashFeed(EventDecoder& decoder, EventRouter& router, NativeSign* signer = nullptr,
            LatencyTracker* tracker = nullptr)
      : decoder_(decoder), router_(router), signer_(signer), tracker_(tracker) {}

  FeedState state() const noexcept { return state_.load(std::memory_order_acquire); }

  Result<void> start() {
    state_.store(FeedState::Connecting, std::memory_order_release);
    // Real: open TLS WebSocket to bc.game with signed query params
    if (signer_ && !signer_->isReady()) {
      auto sig = signer_->signSocketQuery();
      if (!sig.valid) {
        state_.store(FeedState::Degraded, std::memory_order_release);
        return Error{ErrorCode::SignatureError, "sign not ready"};
      }
    }
    state_.store(FeedState::Connected, std::memory_order_release);
    ++starts_;
    return Result<void>::success();
  }

  void stop() {
    state_.store(FeedState::Stopped, std::memory_order_release);
  }

  /** Inject a raw binary frame (tests, replay, live socket callback). */
  Result<CrashEvent> onFrame(const std::uint8_t* data, std::size_t len) {
    ++frames_;
    auto ev = decoder_.decodeBinary(data, len, EventSource::NativeSocket);
    if (!ev) {
      ++decode_errors_;
      return ev.error();
    }
    last_event_ms_.store(nowMs(), std::memory_order_relaxed);
    router_.route(ev.value());
    if (on_event_) on_event_(ev.value());
    return ev;
  }

  void setEventHandler(FrameHandler h) { on_event_ = std::move(h); }

  ConnectionHealth health() const {
    ConnectionHealth h;
    h.connected = state() == FeedState::Connected;
    h.lastMessageMs = last_event_ms_.load(std::memory_order_relaxed);
    h.reconnects = reconnects_.load(std::memory_order_relaxed);
    return h;
  }

  std::uint64_t frameCount() const noexcept { return frames_.load(std::memory_order_relaxed); }
  std::uint64_t decodeErrors() const noexcept { return decode_errors_.load(std::memory_order_relaxed); }

private:
  EventDecoder& decoder_;
  EventRouter& router_;
  NativeSign* signer_ = nullptr;
  LatencyTracker* tracker_ = nullptr;
  FrameHandler on_event_;
  std::atomic<FeedState> state_{FeedState::Stopped};
  std::atomic<TimestampMs> last_event_ms_{0};
  std::atomic<std::uint64_t> frames_{0}, decode_errors_{0}, starts_{0}, reconnects_{0};
};

} // namespace crashcore
