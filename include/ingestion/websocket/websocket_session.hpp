#pragma once
#include <thread>
#include <chrono>
/**
 * Session ties WebSocketConnection + SocketIoClient: connect, join namespace,
 * pump frames, reconnect with backoff.
 */
#include "ingestion/websocket/websocket_connection.hpp"
#include "ingestion/websocket/websocket_metrics.hpp"
#include "ingestion/socketio/socketio_client.hpp"
#include "common/result.hpp"
#include "common/constants.hpp"
#include "timing/timestamp.hpp"
#include <atomic>
#include <memory>
#include <thread>
#include <cmath>

namespace crashcore {

class WebSocketSession {
public:
  WebSocketSession(std::unique_ptr<WebSocketConnection> conn, SocketIoClient& sio,
                   WebSocketMetrics* metrics = nullptr)
      : conn_(std::move(conn)), sio_(sio), metrics_(metrics) {
    conn_->setDataHandler([this](const std::uint8_t* d, std::size_t n) {
      sio_.onBinaryFrame(d, n);
    });
    conn_->setErrorHandler([this](const std::string& e) {
      last_error_ = e;
    });
    conn_->setStateHandler([this](bool up) {
      if (!up) connected_.store(false, std::memory_order_release);
    });
    sio_.setSend([this](const std::vector<std::uint8_t>& f) {
      auto r = conn_->send(f);
      return static_cast<bool>(r);
    });
  }

  ~WebSocketSession() { stop(); }

  Result<void> start() {
    if (running_.exchange(true)) return Result<void>::success();
    auto r = doConnect();
    if (!r) {
      running_.store(false);
      return r;
    }
    if (conn_->config().auto_reconnect) {
      pump_ = std::jthread([this](std::stop_token st) { pumpLoop(st); });
    }
    return Result<void>::success();
  }

  void stop() {
    running_.store(false, std::memory_order_release);
    if (pump_.joinable()) {
      pump_.request_stop();
      pump_.join();
    }
    conn_->close();
    connected_.store(false, std::memory_order_release);
  }

  WebSocketConnection& connection() { return *conn_; }
  const WebSocketConnection& connection() const { return *conn_; }
  bool connected() const noexcept { return connected_.load(std::memory_order_acquire); }
  bool running() const noexcept { return running_.load(std::memory_order_acquire); }
  std::string lastError() const { return last_error_; }
  std::uint64_t reconnectCount() const noexcept {
    return reconnects_.load(std::memory_order_relaxed);
  }

private:
  Result<void> doConnect() {
    auto r = conn_->connect();
    if (!r) {
      if (metrics_) metrics_->handshake_fail.fetch_add(1, std::memory_order_relaxed);
      return r;
    }
    auto j = sio_.joinCrashNamespace();
    if (!j) {
      conn_->close();
      return j;
    }
    connected_.store(true, std::memory_order_release);
    consecutive_fails_ = 0;
    return Result<void>::success();
  }

  void pumpLoop(std::stop_token st) {
    while (!st.stop_requested() && running_.load(std::memory_order_acquire)) {
      conn_->poll();
      sio_.tick();
      if (!conn_->connected() && conn_->config().auto_reconnect) {
        connected_.store(false, std::memory_order_release);
        attemptReconnect(st);
      }
      // Phase 0.16: 1 ms when connected, 50 ms otherwise; honor stop_token
      const auto sleep_ms = conn_->connected() ? 1 : 50;
      for (int i = 0; i < sleep_ms && !st.stop_requested(); ++i)
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
  }

  void attemptReconnect(std::stop_token st) {
    const auto& cfg = conn_->config();
    if (consecutive_fails_ >= cfg.max_reconnect_attempts) return;
    const auto delay = std::min(
        cfg.reconnect_max_ms,
        cfg.reconnect_base_ms * (std::int64_t{1} << std::min(consecutive_fails_, 10)));
    for (std::int64_t waited = 0; waited < delay && !st.stop_requested(); waited += 50) {
      std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    if (st.stop_requested()) return;
    ++consecutive_fails_;
    reconnects_.fetch_add(1, std::memory_order_relaxed);
    if (metrics_) metrics_->reconnects.fetch_add(1, std::memory_order_relaxed);
    doConnect();
  }

  std::unique_ptr<WebSocketConnection> conn_;
  SocketIoClient& sio_;
  WebSocketMetrics* metrics_ = nullptr;
  std::atomic<bool> running_{false};
  std::atomic<bool> connected_{false};
  std::atomic<std::uint64_t> reconnects_{0};
  int consecutive_fails_ = 0;
  std::string last_error_;
  std::jthread pump_;
};

} // namespace crashcore
