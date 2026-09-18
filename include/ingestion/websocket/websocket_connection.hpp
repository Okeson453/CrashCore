#pragma once
/**
 * WebSocket transport abstraction.
 * Default implementation is injectable (tests/replay).
 * Optional CRASHCORE_HAS_BEAST build uses Boost.Beast TLS client.
 */
#include "ingestion/websocket/websocket_config.hpp"
#include "ingestion/websocket/websocket_metrics.hpp"
#include "common/result.hpp"
#include "timing/timestamp.hpp"
#include <atomic>
#include <functional>
#include <mutex>
#include <string>
#include <vector>
#include <deque>

namespace crashcore {

class WebSocketConnection {
public:
  using DataHandler = std::function<void(const std::uint8_t*, std::size_t)>;
  using ErrorHandler = std::function<void(const std::string&)>;
  using StateHandler = std::function<void(bool connected)>;

  explicit WebSocketConnection(WebSocketConfig cfg, WebSocketMetrics* m = nullptr)
      : cfg_(std::move(cfg)), metrics_(m) {}

  virtual ~WebSocketConnection() { close(); }

  WebSocketConnection(const WebSocketConnection&) = delete;
  WebSocketConnection& operator=(const WebSocketConnection&) = delete;

  virtual Result<void> connect() {
    if (connected_.load(std::memory_order_acquire)) return Result<void>::success();
    // Default: mark connected for inject/replay path. Beast override does real TLS.
    connected_.store(true, std::memory_order_release);
    if (metrics_) {
      metrics_->handshake_ok.fetch_add(1, std::memory_order_relaxed);
      metrics_->connected_since_ms.store(nowMs(), std::memory_order_relaxed);
    }
    if (on_state_) on_state_(true);
    return Result<void>::success();
  }

  virtual void close() {
    const bool was = connected_.exchange(false, std::memory_order_acq_rel);
    if (was && on_state_) on_state_(false);
  }

  virtual Result<void> send(const std::vector<std::uint8_t>& data) {
    if (!connected_.load(std::memory_order_acquire)) {
      return Error{ErrorCode::ConnectionFailed, "not connected"};
    }
    if (metrics_) metrics_->onTx(data.size());
    // Default transport: queue outbound for inspection / mock echo
    {
      std::lock_guard lk(tx_mu_);
      tx_log_.push_back(data);
      if (tx_log_.size() > 256) tx_log_.pop_front();
    }
    return Result<void>::success();
  }

  virtual void poll() {
    // Beast implementation pumps the io_context here
  }

  void setDataHandler(DataHandler h) { on_data_ = std::move(h); }
  void setErrorHandler(ErrorHandler h) { on_error_ = std::move(h); }
  void setStateHandler(StateHandler h) { on_state_ = std::move(h); }

  bool connected() const noexcept { return connected_.load(std::memory_order_acquire); }
  const WebSocketConfig& config() const noexcept { return cfg_; }

  /** Inject inbound frame (tests, replay, alternative transports). */
  void inject(const std::uint8_t* d, std::size_t n) {
    if (metrics_) metrics_->onRx(n);
    if (on_data_) on_data_(d, n);
  }

  void inject(const std::vector<std::uint8_t>& d) { inject(d.data(), d.size()); }

  std::size_t txLogSize() const {
    std::lock_guard lk(tx_mu_);
    return tx_log_.size();
  }

  std::vector<std::uint8_t> lastTx() const {
    std::lock_guard lk(tx_mu_);
    if (tx_log_.empty()) return {};
    return tx_log_.back();
  }

protected:
  void reportError(const std::string& msg) {
    if (metrics_) metrics_->errors.fetch_add(1, std::memory_order_relaxed);
    if (on_error_) on_error_(msg);
  }

  WebSocketConfig cfg_;
  WebSocketMetrics* metrics_ = nullptr;
  DataHandler on_data_;
  ErrorHandler on_error_;
  StateHandler on_state_;
  std::atomic<bool> connected_{false};
  mutable std::mutex tx_mu_;
  std::deque<std::vector<std::uint8_t>> tx_log_;
};

} // namespace crashcore
