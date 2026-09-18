#pragma once
#include "ingestion/socketio/socketio_packet.hpp"
#include <cstdint>
#include <cstring>
#include <string>
#include <string_view>
#include <vector>

namespace crashcore {

/**
 * Engine.IO v3/v4 frame helpers.
 * Mapped from TestingEngine native-protocol.ts isEngine* functions.
 */
namespace engineio {

inline bool isOpenFrame(const std::uint8_t* buf, std::size_t len) noexcept {
  // "0{..." — type 0 (open) + JSON
  return len > 1 && buf[0] == 0x30 && buf[1] == 0x7b;
}

inline bool isPing(const std::uint8_t* buf, std::size_t len) noexcept {
  return len == 1 && buf[0] == 0x32; // '2'
}

inline bool isPong(const std::uint8_t* buf, std::size_t len) noexcept {
  return len == 1 && buf[0] == 0x33; // '3'
}

inline bool isMessage(const std::uint8_t* buf, std::size_t len) noexcept {
  return len >= 1 && buf[0] == 0x34; // '4'
}

inline bool isClose(const std::uint8_t* buf, std::size_t len) noexcept {
  return len >= 1 && buf[0] == 0x31; // '1'
}

inline std::vector<std::uint8_t> encodePing() {
  return {0x32};
}

inline std::vector<std::uint8_t> encodePong() {
  return {0x33};
}

/** Parse open packet JSON for sid / pingInterval / pingTimeout (minimal). */
inline SocketIoPacket parseOpen(const std::uint8_t* buf, std::size_t len) {
  SocketIoPacket p;
  p.kind = PacketKind::Open;
  if (len < 2) return p;
  // Skip leading '0'
  std::string_view json(reinterpret_cast<const char*>(buf + 1), len - 1);
  // Extremely light extraction — production may use simdjson
  auto findStr = [&](std::string_view key) -> std::string {
    auto pos = json.find(key);
    if (pos == std::string_view::npos) return {};
    pos = json.find('"', pos + key.size());
    if (pos == std::string_view::npos) return {};
    auto end = json.find('"', pos + 1);
    if (end == std::string_view::npos) return {};
    return std::string(json.substr(pos + 1, end - pos - 1));
  };
  auto findNum = [&](std::string_view key) -> int {
    auto pos = json.find(key);
    if (pos == std::string_view::npos) return 0;
    pos += key.size();
    while (pos < json.size() && (json[pos] == ':' || json[pos] == ' ')) ++pos;
    int v = 0;
    while (pos < json.size() && json[pos] >= '0' && json[pos] <= '9') {
      v = v * 10 + (json[pos] - '0');
      ++pos;
    }
    return v;
  };
  p.sid = findStr("\"sid\"");
  p.pingInterval = findNum("\"pingInterval\"");
  p.pingTimeout  = findNum("\"pingTimeout\"");
  return p;
}

} // namespace engineio
} // namespace crashcore
