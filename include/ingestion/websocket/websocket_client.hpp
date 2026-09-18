#pragma once
/**
 * Top-level WebSocket client: owns metrics, connection, session, and Socket.IO.
 */
#include "ingestion/websocket/websocket_session.hpp"
#include "ingestion/websocket/beast_connection.hpp"
#include "ingestion/websocket/websocket_metrics.hpp"
#include "ingestion/websocket/websocket_config.hpp"
#include "ingestion/socketio/socketio_client.hpp"
#include "ingestion/signing/native_sign.hpp"
#include "common/result.hpp"
#include <atomic>
#include <memory>

namespace crashcore {

class WebSocketClient {
public:
  WebSocketClient(WebSocketConfig cfg, SocketIoClient& sio, NativeSign* sign = nullptr)
      : cfg_(std::move(cfg)), sio_(sio), sign_(sign) {
    // TE: p/t from sign + EIO=3&transport=websocket (Accept-Language etc. go in headers)
    std::string qs = "EIO=3&transport=websocket";
    if (sign_) {
      auto q = sign_->signSocketQuery();
      if (q.valid) {
        qs = "p=" + q.p + "&t=" + q.t + "&" + qs;
      }
    }
    cfg_.extra_query = std::move(qs);
  }

  Result<void> start() {
    metrics_ = std::make_unique<WebSocketMetrics>();
    auto conn = makeWebSocketConnection(cfg_, metrics_.get(), true);
    session_ = std::make_unique<WebSocketSession>(std::move(conn), sio_, metrics_.get());
    auto r = session_->start();
    if (r) running_.store(true, std::memory_order_release);
    return r;
  }

  void stop() {
    running_.store(false, std::memory_order_release);
    if (session_) session_->stop();
  }

  /** Inject frame into live session (tests / alternative ingress). */
  void inject(const std::uint8_t* d, std::size_t n) {
    if (session_) session_->connection().inject(d, n);
  }

  WebSocketSession* session() noexcept { return session_.get(); }
  WebSocketMetrics* metrics() noexcept { return metrics_.get(); }
  bool running() const noexcept { return running_.load(std::memory_order_acquire); }

private:
  WebSocketConfig cfg_;
  SocketIoClient& sio_;
  NativeSign* sign_ = nullptr;
  std::unique_ptr<WebSocketSession> session_;
  std::unique_ptr<WebSocketMetrics> metrics_;
  std::atomic<bool> running_{false};
};

} // namespace crashcore
