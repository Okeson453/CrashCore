#pragma once
/**
 * Socket.IO client facade for BC.Game crash namespace (/g/cm).
 * Transport is injected so unit tests can drive the protocol without a live
 * connection. Production wires WebSocketConnection (Beast when available).
 * Behaviour from TestingEngine native-socket-client.ts / socket-client.ts.
 */
#include "ingestion/socketio/socketio_protocol.hpp"
#include "ingestion/socketio/engineio.hpp"
#include "ingestion/socketio/socketio_packet.hpp"
#include "ingestion/signing/native_sign.hpp"
#include "ingestion/heartbeat/heartbeat_monitor.hpp"
#include "common/constants.hpp"
#include "common/result.hpp"
#include "timing/timestamp.hpp"
#include <atomic>
#include <functional>
#include <mutex>
#include <string>
#include <vector>

namespace crashcore {

class SocketIoClient {
public:
  using FrameHandler = std::function<void(const std::uint8_t*, std::size_t)>;
  using PacketHandler = std::function<void(const SocketIoPacket&)>;
  using SendFn = std::function<bool(const std::vector<std::uint8_t>&)>;
  using StateHandler = std::function<void(bool connected)>;

  explicit SocketIoClient(NativeSign* sign = nullptr)
      : sign_(sign), heartbeat_() {}

  void setSend(SendFn fn) { send_ = std::move(fn); }
  void setFrameHandler(FrameHandler h) { on_frame_ = std::move(h); }
  void setPacketHandler(PacketHandler h) { on_packet_ = std::move(h); }
  void setStateHandler(StateHandler h) { on_state_ = std::move(h); }

  /** Join crash namespace and room — called after WS open. */
  Result<void> joinCrashNamespace() {
    if (!send_) return Error{ErrorCode::InternalError, "send not configured"};
    auto frame = encodeConnect(std::string(constants::BC_SOCKET_NSP));
    if (!send_(frame)) return Error{ErrorCode::NetworkError, "connect frame send failed"};
    auto join = encodeJoin(std::string(constants::BC_SOCKET_NSP), "join");
    if (!send_(join)) return Error{ErrorCode::NetworkError, "join frame send failed"};
    joined_.store(true, std::memory_order_release);
    connected_.store(true, std::memory_order_release);
    if (on_state_) on_state_(true);
    ++joins_;
    return Result<void>::success();
  }

  /** Handle inbound binary or text Engine.IO frame. */
  void onBinaryFrame(const std::uint8_t* data, std::size_t len) {
    ++frames_rx_;
    if (len == 0) return;

    // Engine.IO control
    if (engineio::isPing(data, len)) {
      heartbeat_.onPingSent(); // remote ping → we respond pong
      if (send_) {
        auto pong = engineio::encodePong();
        send_(pong);
        heartbeat_.onPongReceived();
      }
      return;
    }
    if (engineio::isPong(data, len)) {
      heartbeat_.onPongReceived();
      return;
    }
    if (engineio::isOpenFrame(data, len)) {
      auto open = engineio::parseOpen(data, len);
      {
        std::lock_guard lk(mu_);
        sid_ = open.sid;
        if (open.pingInterval > 0) ping_interval_ms_ = open.pingInterval;
        if (open.pingTimeout > 0) ping_timeout_ms_ = open.pingTimeout;
      }
      ++opens_;
      return;
    }
    if (engineio::isClose(data, len)) {
      connected_.store(false, std::memory_order_release);
      joined_.store(false, std::memory_order_release);
      if (on_state_) on_state_(false);
      return;
    }

    // Application packet
    if (data[0] == 0x04) {
      auto pkt = parsePacket(data, len);
      if (on_packet_) on_packet_(pkt);
    }

    if (on_frame_) on_frame_(data, len);
  }

  /** Periodic tick: send ping if due, check timeout. */
  void tick() {
    if (!connected_.load(std::memory_order_acquire)) return;
    if (heartbeat_.shouldPing() && send_) {
      send_(engineio::encodePing());
      heartbeat_.onPingSent();
    }
    heartbeat_.check();
  }

  SocketIoPacket decode(const std::uint8_t* data, std::size_t len) const {
    return parsePacket(data, len);
  }

  bool connected() const noexcept { return connected_.load(std::memory_order_acquire); }
  bool joined() const noexcept { return joined_.load(std::memory_order_acquire); }
  std::string sid() const { std::lock_guard lk(mu_); return sid_; }

  std::uint64_t framesReceived() const noexcept { return frames_rx_.load(std::memory_order_relaxed); }
  std::uint64_t openCount() const noexcept { return opens_.load(std::memory_order_relaxed); }
  std::uint64_t joinCount() const noexcept { return joins_.load(std::memory_order_relaxed); }

  HeartbeatMonitor& heartbeat() noexcept { return heartbeat_; }

  /** Build signed query string for WS handshake (p/t from NativeSign). */
  std::string buildSignedQuery() const {
    if (!sign_) return "EIO=3&transport=websocket";
    auto sig = sign_->signSocketQuery();
    if (!sig.valid) return "EIO=3&transport=websocket";
    return "EIO=3&transport=websocket&p=" + sig.p + "&t=" + sig.t;
  }

private:
  NativeSign* sign_ = nullptr;
  SendFn send_;
  FrameHandler on_frame_;
  PacketHandler on_packet_;
  StateHandler on_state_;
  HeartbeatMonitor heartbeat_;
  mutable std::mutex mu_;
  std::string sid_;
  int ping_interval_ms_ = 25000;
  int ping_timeout_ms_ = 60000;
  std::atomic<bool> connected_{false};
  std::atomic<bool> joined_{false};
  std::atomic<std::uint64_t> frames_rx_{0};
  std::atomic<std::uint64_t> opens_{0};
  std::atomic<std::uint64_t> joins_{0};
};

} // namespace crashcore
