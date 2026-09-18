#pragma once
/**
 * Native BC.Game crash socket control plane.
 * Behaviour mapped from TestingEngine native-socket-client.ts:
 *   stopped → connecting → socket_open → connected (namespace joined) →
 *   degraded → reconnecting
 * Tracks ED freshness for degraded detection; watchdog for stale streams.
 */
#include "ingestion/websocket/websocket_client.hpp"
#include "ingestion/websocket/websocket_config.hpp"
#include "ingestion/socketio/socketio_client.hpp"
#include "ingestion/event_decoder.hpp"
#include "ingestion/event_router.hpp"
#include "ingestion/signing/native_sign.hpp"
#include "ingestion/crash_feed.hpp"
#include "timing/latency_tracker.hpp"
#include "timing/timestamp.hpp"
#include "common/result.hpp"
#include "common/constants.hpp"
#include "logging/structured_logger.hpp"
#include <atomic>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_set>

namespace crashcore {

enum class NativeSocketStatus : std::uint8_t {
  Stopped = 0,
  Connecting,
  SocketOpen,   // transport up, namespace not joined
  Connected,    // namespace joined + stream live
  Degraded,     // no ED events within degraded window
  Reconnecting,
  WafBlocked
};

inline const char* toString(NativeSocketStatus s) noexcept {
  switch (s) {
    case NativeSocketStatus::Stopped: return "stopped";
    case NativeSocketStatus::Connecting: return "connecting";
    case NativeSocketStatus::SocketOpen: return "socket_open";
    case NativeSocketStatus::Connected: return "connected";
    case NativeSocketStatus::Degraded: return "degraded";
    case NativeSocketStatus::Reconnecting: return "reconnecting";
    case NativeSocketStatus::WafBlocked: return "waf_blocked";
  }
  return "unknown";
}

struct NativeSocketConfig {
  std::string host = "socketv4.bc.game";
  std::string path = "/socket.io/";
  std::string nsp = "/g/cm";
  std::int64_t reconnectBaseMs = 150;
  std::int64_t reconnectMaxMs = 1500;
  std::int64_t wafBackoffMs = 12'000;
  std::int64_t degradedMs = 15'000;   // no crash events → degraded
  std::int64_t staleMs = 90'000;      // watchdog force reconnect
  std::int64_t pingMs = 5'000;
  int maxReconnectAttempts = 100;
};

class NativeBcGameSocket {
public:
  using EventHandler = std::function<void(const CrashEvent&)>;
  using StatusHandler = std::function<void(NativeSocketStatus, std::string_view detail)>;

  NativeBcGameSocket(EventDecoder& decoder, EventRouter& router,
                     NativeSign* sign = nullptr, LatencyTracker* tracker = nullptr,
                     NativeSocketConfig cfg = {})
      : decoder_(decoder)
      , router_(router)
      , sign_(sign)
      , tracker_(tracker)
      , cfg_(std::move(cfg))
      , sio_(sign)
      , log_(ComponentId::WebSocket) {
    tracked_ = {"pr", "bg", "pg", "ed", "st"};
    // Packet path only — do not setFrameHandler to onRawFrame (would recurse:
    // onRawFrame → sio_.onBinaryFrame → on_frame_ → onRawFrame).
    sio_.setPacketHandler([this](const SocketIoPacket& pkt) {
      onPacket(pkt);
    });
    sio_.setStateHandler([this](bool up) {
      if (!up) setStatus(NativeSocketStatus::Reconnecting, "transport down");
    });
  }

  ~NativeBcGameSocket() { stop(); }

  Result<void> start() {
    if (status_.load() != NativeSocketStatus::Stopped &&
        status_.load() != NativeSocketStatus::WafBlocked) {
      return Result<void>::success();
    }
    intentional_stop_.store(false);
    setStatus(NativeSocketStatus::Connecting, "start");
    WebSocketConfig wcfg;
    wcfg.host = cfg_.host;
    wcfg.path = cfg_.path;
    wcfg.auto_reconnect = false; // we own reconnect policy
    wcfg.reconnect_base_ms = cfg_.reconnectBaseMs;
    wcfg.reconnect_max_ms = cfg_.reconnectMaxMs;
    if (sign_) {
      auto sig = sign_->signSocketQuery();
      if (sig.valid) {
        wcfg.extra_query = "EIO=3&transport=websocket&p=" + sig.p + "&t=" + sig.t;
      } else {
        wcfg.extra_query = "EIO=3&transport=websocket";
      }
    } else {
      wcfg.extra_query = "EIO=3&transport=websocket";
    }
    client_ = std::make_unique<WebSocketClient>(wcfg, sio_, sign_);
    auto r = client_->start();
    if (!r) {
      setStatus(NativeSocketStatus::Reconnecting, r.error().message);
      scheduleReconnect(false);
      return r;
    }
    setStatus(NativeSocketStatus::SocketOpen, "transport up");
    // Join is performed inside session; if join succeeded mark connected
    if (sio_.joined()) {
      setStatus(NativeSocketStatus::Connected, "namespace joined");
      joined_at_ms_.store(nowMs());
    }
    startWatchdog();
    return Result<void>::success();
  }

  void stop() {
    intentional_stop_.store(true);
    stopWatchdog();
    if (client_) {
      client_->stop();
      client_.reset();
    }
    setStatus(NativeSocketStatus::Stopped, "stop");
  }

  /** Test/replay inject path. */
  void injectFrame(const std::uint8_t* d, std::size_t n) {
    onRawFrame(d, n);
  }

  void injectFrame(const std::vector<std::uint8_t>& d) {
    injectFrame(d.data(), d.size());
  }

  void onEvent(EventHandler h) {
    std::lock_guard lk(mu_);
    event_handlers_.push_back(std::move(h));
  }

  void onStatus(StatusHandler h) {
    std::lock_guard lk(mu_);
    status_handlers_.push_back(std::move(h));
  }

  NativeSocketStatus status() const noexcept {
    return status_.load(std::memory_order_acquire);
  }

  ConnectionHealth health() const {
    ConnectionHealth h;
    const auto st = status();
    h.connected = (st == NativeSocketStatus::Connected || st == NativeSocketStatus::Degraded);
    h.lastMessageMs = last_event_ms_.load(std::memory_order_relaxed);
    h.reconnects = reconnects_.load(std::memory_order_relaxed);
    h.lastError = last_error_;
    return h;
  }

  std::uint64_t eventsEmitted() const noexcept {
    return events_emitted_.load(std::memory_order_relaxed);
  }
  std::uint64_t framesRx() const noexcept {
    return frames_rx_.load(std::memory_order_relaxed);
  }

  SocketIoClient& socketIo() noexcept { return sio_; }

private:
  void onRawFrame(const std::uint8_t* d, std::size_t n) {
    frames_rx_.fetch_add(1, std::memory_order_relaxed);
    last_frame_ms_.store(nowMs(), std::memory_order_relaxed);
    // Engine.IO open → still SocketOpen until join
    if (engineio::isOpenFrame(d, n)) {
      setStatus(NativeSocketStatus::SocketOpen, "engine open");
    }
    sio_.onBinaryFrame(d, n);
  }

  void onPacket(const SocketIoPacket& pkt) {
    if (pkt.kind == PacketKind::Connect) {
      // namespace connect ack
      if (pkt.nsp == cfg_.nsp || pkt.nsp.empty()) {
        // attempt join if not yet
        if (!sio_.joined()) {
          auto j = sio_.joinCrashNamespace();
          if (j) {
            setStatus(NativeSocketStatus::Connected, "joined");
            joined_at_ms_.store(nowMs());
          }
        }
      }
      return;
    }
    if (pkt.kind != PacketKind::Event) return;
    if (tracked_.count(pkt.event) == 0) return;

    auto decoded = decoder_.decodeEventPacket(pkt, EventSource::NativeSocket, steadyNs());
    if (!decoded) return;
    auto& ev = decoded.value();
    last_event_ms_.store(nowMs(), std::memory_order_relaxed);
    if (ev.kind == EventKind::End) {
      last_ed_ms_.store(nowMs(), std::memory_order_relaxed);
      // ED confirms live stream
      if (status() == NativeSocketStatus::SocketOpen ||
          status() == NativeSocketStatus::Degraded ||
          status() == NativeSocketStatus::Connected) {
        setStatus(NativeSocketStatus::Connected, "ed");
      }
    }
    if (!ev.gameId.empty()) {
      std::lock_guard lk(mu_);
      current_game_id_ = ev.gameId;
    }
    router_.route(ev);
    events_emitted_.fetch_add(1, std::memory_order_relaxed);
    std::vector<EventHandler> handlers;
    {
      std::lock_guard lk(mu_);
      handlers = event_handlers_;
    }
    for (auto& h : handlers) h(ev);
  }

  void setStatus(NativeSocketStatus s, std::string_view detail) {
    const auto prev = status_.exchange(s, std::memory_order_acq_rel);
    if (prev == s && detail.empty()) return;
    last_error_ = std::string(detail);
    log_.info("status ", toString(s), " ", detail);
    std::vector<StatusHandler> handlers;
    {
      std::lock_guard lk(mu_);
      handlers = status_handlers_;
    }
    for (auto& h : handlers) h(s, detail);
  }

  void startWatchdog() {
    if (watchdog_running_.exchange(true)) return;
    watchdog_ = std::jthread([this](std::stop_token st) {
      while (!st.stop_requested() && watchdog_running_.load()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        if (intentional_stop_.load()) break;
        checkDegraded();
        checkStale();
        sio_.tick();
      }
    });
  }

  void stopWatchdog() {
    watchdog_running_.store(false);
    if (watchdog_.joinable()) {
      watchdog_.request_stop();
      watchdog_.join();
    }
  }

  void checkDegraded() {
    const auto st = status();
    if (st != NativeSocketStatus::Connected && st != NativeSocketStatus::Degraded) return;
    const auto last = last_event_ms_.load(std::memory_order_relaxed);
    if (last == 0) return;
    if (nowMs() - last > cfg_.degradedMs) {
      if (st != NativeSocketStatus::Degraded)
        setStatus(NativeSocketStatus::Degraded, "event age");
    }
  }

  void checkStale() {
    const auto st = status();
    if (st == NativeSocketStatus::Stopped || st == NativeSocketStatus::WafBlocked) return;
    const auto last = last_frame_ms_.load(std::memory_order_relaxed);
    if (last == 0) return;
    if (nowMs() - last > cfg_.staleMs) {
      log_.warn("stale stream, reconnecting");
      scheduleReconnect(false);
    }
  }

  void scheduleReconnect(bool waf) {
    if (intentional_stop_.load()) return;
    setStatus(waf ? NativeSocketStatus::WafBlocked : NativeSocketStatus::Reconnecting,
              waf ? "waf" : "reconnect");
    reconnects_.fetch_add(1, std::memory_order_relaxed);
    if (client_) {
      client_->stop();
      client_.reset();
    }
    const auto delay = waf ? cfg_.wafBackoffMs
                           : std::min(cfg_.reconnectMaxMs,
                                      cfg_.reconnectBaseMs * (1 + static_cast<std::int64_t>(
                                          reconnects_.load() % 10)));
    // Phase 0.13: joinable reconnect worker instead of detached thread
    if (reconnect_worker_.joinable()) {
      intentional_stop_.store(true);
      reconnect_worker_.join();
      intentional_stop_.store(false);
    }
    reconnect_worker_ = std::jthread([this, delay](std::stop_token st) {
      for (std::int64_t waited = 0; waited < delay && !st.stop_requested(); waited += 50)
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
      if (st.stop_requested() || intentional_stop_.load()) return;
      start();
    });
  }

  EventDecoder& decoder_;
  EventRouter& router_;
  NativeSign* sign_ = nullptr;
  LatencyTracker* tracker_ = nullptr;
  NativeSocketConfig cfg_;
  std::jthread reconnect_worker_;
  SocketIoClient sio_;
  std::unique_ptr<WebSocketClient> client_;
  StructuredLogger log_;
  std::unordered_set<std::string> tracked_;
  std::mutex mu_;
  std::vector<EventHandler> event_handlers_;
  std::vector<StatusHandler> status_handlers_;
  std::string current_game_id_;
  std::string last_error_;
  std::atomic<NativeSocketStatus> status_{NativeSocketStatus::Stopped};
  std::atomic<TimestampMs> last_event_ms_{0};
  std::atomic<TimestampMs> last_ed_ms_{0};
  std::atomic<TimestampMs> last_frame_ms_{0};
  std::atomic<TimestampMs> joined_at_ms_{0};
  std::atomic<std::uint64_t> frames_rx_{0};
  std::atomic<std::uint64_t> events_emitted_{0};
  std::atomic<std::uint64_t> reconnects_{0};
  std::atomic<bool> intentional_stop_{false};
  std::atomic<bool> watchdog_running_{false};
  std::jthread watchdog_;
};

} // namespace crashcore
