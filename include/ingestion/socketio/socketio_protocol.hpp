#pragma once
#include <cmath>
/**
 * BC.Game Engine.IO custom binary frames + protobuf helpers.
 * Direct port of TestingEngine src/lib/crash/native-protocol.ts behaviour.
 */
#include "ingestion/socketio/socketio_packet.hpp"
#include "common/types.hpp"
#include "timing/timestamp.hpp"
#include <cstdint>
#include <cstring>
#include <string>
#include <string_view>
#include <vector>
#include <optional>
#include <unordered_map>
#include <variant>
#include <utility>

namespace crashcore {

inline std::vector<std::uint8_t> encodeConnect(const std::string& nsp) {
  std::vector<std::uint8_t> out(3 + nsp.size() + 1);
  out[0] = 0x04;
  out[1] = 0x00;
  out[2] = static_cast<std::uint8_t>(nsp.size());
  std::memcpy(out.data() + 3, nsp.data(), nsp.size());
  out[3 + nsp.size()] = 0;
  return out;
}

inline std::vector<std::uint8_t> encodeJoin(const std::string& nsp, const std::string& room = "join") {
  // type 0x82 has the 0x80 flag set → parsePacket skips 4 bytes after type (offset=6)
  // Layout: [0x04][0x82][4 reserved][nspLen][nsp…][roomLen][room…]
  const std::size_t need = 2 + 4 + 1 + nsp.size() + 1 + room.size();
  std::vector<std::uint8_t> out(need, 0);
  out[0] = 0x04;
  out[1] = 0x82;
  out[6] = static_cast<std::uint8_t>(nsp.size());
  std::memcpy(out.data() + 7, nsp.data(), nsp.size());
  const auto evOff = 7 + nsp.size();
  out[evOff] = static_cast<std::uint8_t>(room.size());
  std::memcpy(out.data() + evOff + 1, room.data(), room.size());
  return out;
}

inline std::vector<std::uint8_t> encodeEvent(const std::string& nsp, const std::string& event,
                                             const std::vector<std::uint8_t>& payload) {
  std::vector<std::uint8_t> out(3 + nsp.size() + 1 + event.size() + payload.size());
  out[0] = 0x04;
  out[1] = 0x02;
  out[2] = static_cast<std::uint8_t>(nsp.size());
  std::memcpy(out.data() + 3, nsp.data(), nsp.size());
  std::size_t o = 3 + nsp.size();
  out[o++] = static_cast<std::uint8_t>(event.size());
  std::memcpy(out.data() + o, event.data(), event.size());
  o += event.size();
  if (!payload.empty()) std::memcpy(out.data() + o, payload.data(), payload.size());
  return out;
}

inline SocketIoPacket parsePacket(const std::uint8_t* buf, std::size_t len) {
  SocketIoPacket p;
  if (len < 2 || buf[0] != 0x04) return p;
  const auto type = buf[1];
  const auto base = type & 0x0f;
  std::size_t offset = 2;
  if (type & 0x80) offset += 4;
  if (offset >= len) return p;
  const auto nspLen = buf[offset++];
  if (offset + nspLen > len) return p;
  p.nsp.assign(reinterpret_cast<const char*>(buf + offset), nspLen);
  offset += nspLen;
  if (base == 0) { p.kind = PacketKind::Connect; return p; }
  if (offset >= len) { p.kind = PacketKind::Connect; return p; }
  const auto evLen = buf[offset++];
  if (offset + evLen > len) return p;
  p.event.assign(reinterpret_cast<const char*>(buf + offset), evLen);
  offset += evLen;
  p.kind = PacketKind::Event;
  if (offset < len) p.payload.assign(buf + offset, buf + len);
  return p;
}

inline SocketIoPacket parsePacket(const std::vector<std::uint8_t>& buf) {
  return parsePacket(buf.data(), buf.size());
}

inline bool readVarint(const std::uint8_t* buf, std::size_t len, std::size_t& offset, std::uint64_t& value) {
  value = 0;
  int shift = 0;
  while (offset < len && shift < 56) {
    const auto b = buf[offset++];
    value |= static_cast<std::uint64_t>(b & 0x7f) << shift;
    if ((b & 0x80) == 0) return true;
    shift += 7;
  }
  return false;
}

inline void writeVarint(std::uint64_t n, std::vector<std::uint8_t>& out) {
  while (n > 0x7f) {
    out.push_back(static_cast<std::uint8_t>((n & 0x7f) | 0x80));
    n >>= 7;
  }
  out.push_back(static_cast<std::uint8_t>(n));
}

inline std::unordered_map<int, std::variant<std::uint64_t, std::string>>
decodeProtobuf(const std::uint8_t* buf, std::size_t len) {
  std::unordered_map<int, std::variant<std::uint64_t, std::string>> fields;
  std::size_t offset = 0;
  while (offset < len) {
    std::uint64_t tag = 0;
    if (!readVarint(buf, len, offset, tag)) break;
    const int field = static_cast<int>(tag >> 3);
    const int wire  = static_cast<int>(tag & 7);
    if (wire == 0) {
      std::uint64_t value = 0;
      if (!readVarint(buf, len, offset, value)) break;
      fields[field] = value;
    } else if (wire == 2) {
      std::uint64_t size = 0;
      if (!readVarint(buf, len, offset, size)) break;
      if (offset + size > len) break;
      fields[field] = std::string(reinterpret_cast<const char*>(buf + offset), static_cast<std::size_t>(size));
      offset += static_cast<std::size_t>(size);
    } else {
      break;
    }
  }
  return fields;
}

inline std::optional<std::int64_t> decodeProgressElapsed(const std::uint8_t* buf, std::size_t len) {
  if (len == 0) return std::nullopt;
  if (buf[0] == 8) {
    std::size_t off = 1;
    std::uint64_t v = 0;
    if (readVarint(buf, len, off, v)) return static_cast<std::int64_t>(v);
  }
  auto fields = decodeProtobuf(buf, len);
  auto it = fields.find(1);
  if (it != fields.end() && std::holds_alternative<std::uint64_t>(it->second))
    return static_cast<std::int64_t>(std::get<std::uint64_t>(it->second));
  return std::nullopt;
}

inline std::vector<std::uint8_t> encodeProtobuf(
    const std::vector<std::pair<int, std::variant<std::uint64_t, std::string>>>& fields) {
  std::vector<std::uint8_t> out;
  out.reserve(64);
  for (const auto& [field, value] : fields) {
    if (std::holds_alternative<std::uint64_t>(value)) {
      writeVarint(static_cast<std::uint64_t>((field << 3) | 0), out);
      writeVarint(std::get<std::uint64_t>(value), out);
    } else {
      const auto& s = std::get<std::string>(value);
      writeVarint(static_cast<std::uint64_t>((field << 3) | 2), out);
      writeVarint(static_cast<std::uint64_t>(s.size()), out);
      out.insert(out.end(), s.begin(), s.end());
    }
  }
  return out;
}

struct CrashPayload {
  std::optional<std::string> gameId;
  std::optional<double>      multiplier;
  std::optional<std::string> hash;
  std::optional<std::int64_t> beginTime;
  std::optional<std::int64_t> endTime;
  std::optional<std::int64_t> elapsedMs;
};

inline CrashPayload fieldsToPayload(std::string_view event,
    const std::unordered_map<int, std::variant<std::uint64_t, std::string>>& fields) {
  CrashPayload p;
  if (event == "pg") {
    auto it = fields.find(1);
    if (it != fields.end() && std::holds_alternative<std::uint64_t>(it->second)) {
      p.elapsedMs = static_cast<std::int64_t>(std::get<std::uint64_t>(it->second));
      p.multiplier = multiplierFromElapsed(*p.elapsedMs);
    }
    return p;
  }
  auto getNum = [&](int f) -> std::optional<std::uint64_t> {
    auto it = fields.find(f);
    if (it != fields.end() && std::holds_alternative<std::uint64_t>(it->second))
      return std::get<std::uint64_t>(it->second);
    return std::nullopt;
  };
  auto getStr = [&](int f) -> std::optional<std::string> {
    auto it = fields.find(f);
    if (it != fields.end() && std::holds_alternative<std::string>(it->second))
      return std::get<std::string>(it->second);
    return std::nullopt;
  };
  if (auto g = getNum(1)) p.gameId = std::to_string(*g);
  else if (auto gs = getStr(1)) p.gameId = *gs;
  if (auto m = getNum(6)) p.multiplier = static_cast<double>(*m) / 100.0; // hundredths
  p.hash = getStr(7);
  if (auto b = getNum(3)) p.beginTime = static_cast<std::int64_t>(*b);
  else if (auto b4 = getNum(4)) p.beginTime = static_cast<std::int64_t>(*b4);
  if (event == "ed" || event == "st") p.endTime = nowMs();
  return p;
}

/** Build end-event protobuf for tests/replay: gameId, multiplier hundredths, optional hash. */
inline std::vector<std::uint8_t> encodeEndPayload(std::uint64_t gameId, double mult,
                                                   std::string_view hash = {}) {
  std::vector<std::pair<int, std::variant<std::uint64_t, std::string>>> fields;
  fields.emplace_back(1, gameId);
  fields.emplace_back(6, static_cast<std::uint64_t>(std::llround(mult * 100.0)));
  if (!hash.empty()) fields.emplace_back(7, std::string(hash));
  return encodeProtobuf(fields);
}

/** Build progress payload with elapsed ms. */
inline std::vector<std::uint8_t> encodeProgressPayload(std::int64_t elapsedMs) {
  std::vector<std::pair<int, std::variant<std::uint64_t, std::string>>> fields;
  fields.emplace_back(1, static_cast<std::uint64_t>(elapsedMs));
  return encodeProtobuf(fields);
}

} // namespace crashcore
