#pragma once
/**
 * Boost.Beast TLS WebSocket client for BC.Game.
 * Production transport replacing the recording stub.
 */
#include "ingestion/websocket/websocket_connection.hpp"
#include "ingestion/websocket/websocket_config.hpp"
#include "ingestion/websocket/websocket_metrics.hpp"
#include "common/result.hpp"
#include "common/errors.hpp"
#include "timing/timestamp.hpp"

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>

#include <atomic>
#include <chrono>
#include <deque>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

namespace crashcore {

namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;
namespace net = boost::asio;
namespace ssl = boost::asio::ssl;
using tcp = boost::asio::ip::tcp;

class BeastWebSocketConnection : public WebSocketConnection {
public:
  explicit BeastWebSocketConnection(WebSocketConfig cfg, WebSocketMetrics* m = nullptr)
      : WebSocketConnection(std::move(cfg), m)
      , ioc_()
      , ssl_ctx_(ssl::context::tlsv12_client)
      , resolver_(ioc_) {
    ssl_ctx_.set_default_verify_paths();
    ssl_ctx_.set_verify_mode(ssl::verify_peer);
  }

  ~BeastWebSocketConnection() override {
    close();
    if (io_thread_.joinable()) {
      ioc_.stop();
      io_thread_.join();
    }
  }

  Result<void> connect() override {
    if (connected_.load(std::memory_order_acquire)) return Result<void>::success();

    try {
      // Build host/port/target from config
      const std::string host = cfg_.host.empty() ? "api.bc.game" : cfg_.host;
      const std::string port = cfg_.port.empty() ? "443" : cfg_.port;
      std::string target = cfg_.path.empty() ? "/socket.io/" : cfg_.path;
      if (!cfg_.extra_query.empty()) {
        target += (target.find('?') == std::string::npos ? "?" : "&") + cfg_.extra_query;
      }

      auto const results = resolver_.resolve(host, port);

      stream_ = std::make_unique<websocket::stream<beast::ssl_stream<beast::tcp_stream>>>(
          ioc_, ssl_ctx_);

      // Set SNI
      if (!SSL_set_tlsext_host_name(stream_->next_layer().native_handle(), host.c_str())) {
        return Error{ErrorCode::ConnectionFailed, "SNI set failed"};
      }

      beast::get_lowest_layer(*stream_).expires_after(std::chrono::seconds(15));
      beast::get_lowest_layer(*stream_).connect(results);

      stream_->next_layer().handshake(ssl::stream_base::client);

      // WebSocket handshake
      stream_->set_option(websocket::stream_base::decorator(
          [&](websocket::request_type& req) {
            req.set(http::field::user_agent, cfg_.user_agent.empty()
                ? "CrashCore/1.0" : cfg_.user_agent);
            req.set(http::field::origin, "https://bc.game");
            req.set(http::field::referer, "https://bc.game/");
            req.set(http::field::accept_language, "en");
          }));

      stream_->handshake(host, target);
      stream_->binary(true);
      stream_->read_message_max(1 << 20); // 1 MiB

      connected_.store(true, std::memory_order_release);
      if (metrics_) {
        metrics_->handshake_ok.fetch_add(1, std::memory_order_relaxed);
        metrics_->connected_since_ms.store(nowMs(), std::memory_order_relaxed);
      }
      if (on_state_) on_state_(true);

      // Start async read loop on io thread
      running_.store(true, std::memory_order_release);
      io_thread_ = std::thread([this] {
        doRead();
        ioc_.run();
      });

      return Result<void>::success();
    } catch (const std::exception& ex) {
      connected_.store(false, std::memory_order_release);
      reportError(std::string("Beast connect: ") + ex.what());
      return Error{ErrorCode::ConnectionFailed, ex.what()};
    }
  }

  void close() override {
    running_.store(false, std::memory_order_release);
    const bool was = connected_.exchange(false, std::memory_order_acq_rel);
    if (was && stream_) {
      try {
        beast::get_lowest_layer(*stream_).expires_after(std::chrono::seconds(3));
        stream_->close(websocket::close_code::normal);
      } catch (...) {}
    }
    ioc_.stop();
    if (io_thread_.joinable()) io_thread_.join();
    stream_.reset();
    ioc_.restart();
    if (was && on_state_) on_state_(false);
  }

  Result<void> send(const std::vector<std::uint8_t>& data) override {
    if (!connected_.load(std::memory_order_acquire) || !stream_) {
      return Error{ErrorCode::ConnectionFailed, "not connected"};
    }
    try {
      stream_->write(net::buffer(data));
      if (metrics_) metrics_->onTx(data.size());
      {
        std::lock_guard lk(tx_mu_);
        tx_log_.push_back(data);
        if (tx_log_.size() > 256) tx_log_.pop_front();
      }
      return Result<void>::success();
    } catch (const std::exception& ex) {
      reportError(std::string("Beast send: ") + ex.what());
      return Error{ErrorCode::NetworkError, ex.what()};
    }
  }

  void poll() override {
    // Async model — no-op; io_thread pumps
  }

private:
  void doRead() {
    if (!stream_ || !running_.load(std::memory_order_acquire)) return;
    stream_->async_read(buffer_,
        [this](beast::error_code ec, std::size_t bytes) {
          if (ec) {
            if (ec != websocket::error::closed && running_.load(std::memory_order_acquire)) {
              reportError(ec.message());
            }
            connected_.store(false, std::memory_order_release);
            if (on_state_) on_state_(false);
            return;
          }
          if (metrics_) metrics_->onRx(bytes);
          auto data = buffer_.cdata();
          if (on_data_ && bytes > 0) {
            on_data_(static_cast<const std::uint8_t*>(data.data()), bytes);
          }
          buffer_.consume(buffer_.size());
          doRead();
        });
  }

  net::io_context ioc_;
  ssl::context ssl_ctx_;
  tcp::resolver resolver_;
  std::unique_ptr<websocket::stream<beast::ssl_stream<beast::tcp_stream>>> stream_;
  beast::flat_buffer buffer_;
  std::thread io_thread_;
  std::atomic<bool> running_{false};
};

/** Factory selecting Beast when CRASHCORE_USE_BEAST or always prefer real transport. */
inline std::unique_ptr<WebSocketConnection> makeWebSocketConnection(
    WebSocketConfig cfg, WebSocketMetrics* m = nullptr, bool useBeast = true) {
  if (useBeast) {
    return std::make_unique<BeastWebSocketConnection>(std::move(cfg), m);
  }
  return std::make_unique<WebSocketConnection>(std::move(cfg), m);
}

} // namespace crashcore
