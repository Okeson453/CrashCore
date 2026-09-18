#pragma once
/**
 * PORT_BEHAVIOR handshake URL construction from TestingEngine
 * bcgame-crash-transport.ts / native-socket-client.ts
 */
#include "ingestion/signing/native_sign.hpp"
#include "ingestion/websocket/websocket_config.hpp"
#include "ingestion/reconnect_policy.hpp"
#include "common/constants.hpp"
#include "common/result.hpp"
#include <string>
#include <sstream>
#include <vector>

namespace crashcore {

struct HandshakeParams {
  std::string host = "socketv4.bc.game";
  std::string path = "/socket.io/";
  std::string eio = "3";
  std::string transport = "websocket";
  std::string origin = "https://bc.game";
  std::string userAgent = std::string(constants::DEFAULT_UA);
  bool secure = true;
};

inline std::string buildSocketPath(const HandshakeParams& p, const SocketSignature& sig) {
  std::ostringstream oss;
  oss << p.path;
  char sep = (p.path.find('?') == std::string::npos) ? '?' : '&';
  oss << sep << "EIO=" << p.eio << "&transport=" << p.transport;
  if (sig.valid) {
    oss << "&p=" << sig.p << "&t=" << sig.t;
  }
  return oss.str();
}

inline WebSocketConfig makeWsConfig(const HandshakeParams& hp, const SocketSignature& sig,
                                    const ReconnectPolicy& policy = {}) {
  WebSocketConfig cfg;
  cfg.host = hp.host;
  cfg.path = buildSocketPath(hp, sig);
  cfg.port = hp.secure ? "443" : "80";
  cfg.tls = hp.secure;
  cfg.user_agent = hp.userAgent;
  cfg.auto_reconnect = true;
  cfg.reconnect_base_ms = policy.baseMs;
  cfg.reconnect_max_ms = policy.maxMs;
  cfg.max_reconnect_attempts = policy.maxAttempts;
  if (sig.valid) {
    cfg.extra_query = "p=" + sig.p + "&t=" + sig.t;
  }
  return cfg;
}

inline std::vector<std::pair<std::string, std::string>> handshakeHeaders(const HandshakeParams& hp) {
  return {
    {"Origin", hp.origin},
    {"User-Agent", hp.userAgent},
    {"Cache-Control", "no-cache"},
    {"Pragma", "no-cache"},
    {"Accept-Encoding", "gzip, deflate, br"},
    {"Accept-Language", "en-US,en;q=0.9"},
  };
}

enum class HandshakeStage : std::uint8_t {
  Idle = 0,
  ResolvingSign,
  OpeningTcp,
  TlsHandshake,
  HttpUpgrade,
  EngineOpen,
  NamespaceConnect,
  NamespaceJoin,
  Live,
  Failed
};

inline const char* toString(HandshakeStage s) noexcept {
  switch (s) {
    case HandshakeStage::Idle: return "idle";
    case HandshakeStage::ResolvingSign: return "resolving_sign";
    case HandshakeStage::OpeningTcp: return "opening_tcp";
    case HandshakeStage::TlsHandshake: return "tls_handshake";
    case HandshakeStage::HttpUpgrade: return "http_upgrade";
    case HandshakeStage::EngineOpen: return "engine_open";
    case HandshakeStage::NamespaceConnect: return "namespace_connect";
    case HandshakeStage::NamespaceJoin: return "namespace_join";
    case HandshakeStage::Live: return "live";
    case HandshakeStage::Failed: return "failed";
  }
  return "idle";
}

class HandshakeStateMachine {
public:
  HandshakeStage stage() const noexcept { return stage_; }
  void advance(HandshakeStage s) { stage_ = s; ++transitions_; }
  void fail(std::string reason) {
    stage_ = HandshakeStage::Failed;
    last_error_ = std::move(reason);
    ++failures_;
  }
  const std::string& lastError() const noexcept { return last_error_; }
  std::uint64_t transitionCount() const noexcept { return transitions_; }
  std::uint64_t failureCount() const noexcept { return failures_; }
  bool isLive() const noexcept { return stage_ == HandshakeStage::Live; }

  /** Drive a successful offline handshake simulation (no network). */
  Result<void> simulateSuccess(NativeSign* sign = nullptr) {
    advance(HandshakeStage::ResolvingSign);
    if (sign) {
      auto sig = sign->signSocketQuery();
      if (!sig.valid) {
        // soft-fail continue without sign in offline mode
      }
    }
    advance(HandshakeStage::OpeningTcp);
    advance(HandshakeStage::TlsHandshake);
    advance(HandshakeStage::HttpUpgrade);
    advance(HandshakeStage::EngineOpen);
    advance(HandshakeStage::NamespaceConnect);
    advance(HandshakeStage::NamespaceJoin);
    advance(HandshakeStage::Live);
    return Result<void>::success();
  }

private:
  HandshakeStage stage_ = HandshakeStage::Idle;
  std::string last_error_;
  std::uint64_t transitions_ = 0;
  std::uint64_t failures_ = 0;
};

} // namespace crashcore
