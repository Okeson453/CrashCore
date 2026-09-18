#pragma once
/**
 * Native BC.Game transport facade combining sign + websocket + socketio + feed.
 * Maps TestingEngine bcgame-crash-transport.ts / native-socket-client.ts.
 */
#include "ingestion/crash_feed.hpp"
#include "ingestion/signing/native_sign.hpp"
#include "ingestion/socketio/socketio_client.hpp"
#include "ingestion/websocket/websocket_client.hpp"
#include "ingestion/event_decoder.hpp"
#include "ingestion/event_router.hpp"
#include "security/secret_provider.hpp"
#include "timing/latency_tracker.hpp"
#include "common/result.hpp"
#include <memory>
#include <atomic>

namespace crashcore {

class NativeTransport {
public:
  NativeTransport(SecretProvider& secrets, EventDecoder& decoder, EventRouter& router,
                  LatencyTracker* tracker = nullptr)
      : secrets_(secrets)
      , sign_(secrets)
      , sio_(&sign_)
      , decoder_(decoder)
      , router_(router)
      , feed_(decoder, router, &sign_, tracker)
      , tracker_(tracker) {}

  Result<void> start() {
    WebSocketConfig cfg;
    cfg.auto_reconnect = true;
    auto q = sign_.signSocketQuery();
    if (q.valid) {
      cfg.extra_query = "p=" + q.p + "&t=" + q.t;
    }
    ws_ = std::make_unique<WebSocketClient>(cfg, sio_, &sign_);
    auto r = ws_->start();
    if (!r) return r;
    // Wire socketio frames into feed
    sio_.setFrameHandler([this](const std::uint8_t* d, std::size_t n) {
      feed_.onFrame(d, n);
    });
    auto fr = feed_.start();
    if (!fr) return fr;
    running_.store(true);
    return Result<void>::success();
  }

  void stop() {
    running_.store(false);
    if (ws_) ws_->stop();
    feed_.stop();
  }

  void inject(const std::uint8_t* d, std::size_t n) {
    if (ws_) ws_->inject(d, n);
    else feed_.onFrame(d, n);
  }

  bool running() const noexcept { return running_.load(); }
  CrashFeed& feed() noexcept { return feed_; }
  SocketIoClient& socketIo() noexcept { return sio_; }
  NativeSign& sign() noexcept { return sign_; }
  WebSocketClient* websocket() noexcept { return ws_.get(); }

private:
  SecretProvider& secrets_;
  NativeSign sign_;
  SocketIoClient sio_;
  EventDecoder& decoder_;
  EventRouter& router_;
  CrashFeed feed_;
  LatencyTracker* tracker_ = nullptr;
  std::unique_ptr<WebSocketClient> ws_;
  std::atomic<bool> running_{false};
};

} // namespace crashcore
