#pragma once
#include "common/constants.hpp"
#include <cstdint>
#include <string>

namespace crashcore {

struct WebSocketConfig {
  std::string host = std::string(constants::BC_GAME_HOST);
  std::string path = "/socket.io/?EIO=3&transport=websocket";
  std::string port = "443";
  bool tls = true;
  std::int64_t handshake_timeout_ms = 10'000;
  std::int64_t read_timeout_ms = 60'000;
  std::int64_t reconnect_base_ms = constants::RECONNECT_BASE_MS;
  std::int64_t reconnect_max_ms = constants::RECONNECT_MAX_MS;
  int max_reconnect_attempts = 50;
  std::string user_agent = std::string(constants::DEFAULT_UA);
  bool auto_reconnect = true;
  /** Extra query params (e.g. signed p/t) appended to path. */
  std::string extra_query;
};

inline std::string buildWebSocketUrl(const WebSocketConfig& cfg) {
  std::string url = cfg.tls ? "wss://" : "ws://";
  url += cfg.host;
  if (cfg.port != "443" && cfg.port != "80") {
    url += ":";
    url += cfg.port;
  }
  url += cfg.path;
  if (!cfg.extra_query.empty()) {
    url += (cfg.path.find('?') != std::string::npos) ? "&" : "?";
    url += cfg.extra_query;
  }
  return url;
}

} // namespace crashcore
