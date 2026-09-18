#pragma once
#include <cstdint>
#include <cstring>
#include <string>
#include <vector>
#include <optional>

namespace crashcore::ws {

enum class Opcode : std::uint8_t {
  Continuation = 0x0, Text = 0x1, Binary = 0x2, Close = 0x8, Ping = 0x9, Pong = 0xA
};

struct Frame {
  bool fin = true;
  Opcode opcode = Opcode::Binary;
  std::vector<std::uint8_t> payload;
  bool masked = false;
};

inline std::vector<std::uint8_t> encodeFrame(const Frame& f) {
  std::vector<std::uint8_t> out;
  std::uint8_t b0 = (f.fin ? 0x80 : 0) | (static_cast<std::uint8_t>(f.opcode) & 0x0f);
  out.push_back(b0);
  const auto len = f.payload.size();
  if (len < 126) {
    out.push_back(static_cast<std::uint8_t>(len) | (f.masked ? 0x80 : 0));
  } else if (len <= 0xffff) {
    out.push_back(126 | (f.masked ? 0x80 : 0));
    out.push_back(static_cast<std::uint8_t>((len >> 8) & 0xff));
    out.push_back(static_cast<std::uint8_t>(len & 0xff));
  } else {
    out.push_back(127 | (f.masked ? 0x80 : 0));
    for (int i = 7; i >= 0; --i)
      out.push_back(static_cast<std::uint8_t>((static_cast<std::uint64_t>(len) >> (i * 8)) & 0xff));
  }
  if (f.masked) {
    std::uint8_t mask[4] = {0x12, 0x34, 0x56, 0x78};
    out.insert(out.end(), mask, mask + 4);
    for (std::size_t i = 0; i < f.payload.size(); ++i)
      out.push_back(f.payload[i] ^ mask[i % 4]);
  } else {
    out.insert(out.end(), f.payload.begin(), f.payload.end());
  }
  return out;
}

inline std::optional<Frame> decodeFrame(const std::uint8_t* data, std::size_t len) {
  if (len < 2) return std::nullopt;
  Frame f;
  f.fin = (data[0] & 0x80) != 0;
  f.opcode = static_cast<Opcode>(data[0] & 0x0f);
  f.masked = (data[1] & 0x80) != 0;
  std::uint64_t payloadLen = data[1] & 0x7f;
  std::size_t off = 2;
  if (payloadLen == 126) {
    if (len < 4) return std::nullopt;
    payloadLen = (static_cast<std::uint64_t>(data[2]) << 8) | data[3];
    off = 4;
  } else if (payloadLen == 127) {
    if (len < 10) return std::nullopt;
    payloadLen = 0;
    for (int i = 0; i < 8; ++i) payloadLen = (payloadLen << 8) | data[2 + i];
    off = 10;
  }
  std::uint8_t mask[4]{};
  if (f.masked) {
    if (len < off + 4) return std::nullopt;
    std::memcpy(mask, data + off, 4);
    off += 4;
  }
  if (len < off + payloadLen) return std::nullopt;
  f.payload.resize(static_cast<std::size_t>(payloadLen));
  for (std::size_t i = 0; i < payloadLen; ++i) {
    auto b = data[off + i];
    f.payload[i] = f.masked ? (b ^ mask[i % 4]) : b;
  }
  return f;
}

inline std::vector<std::uint8_t> encodeBinary(const std::vector<std::uint8_t>& payload, bool mask = false) {
  Frame f; f.opcode = Opcode::Binary; f.payload = payload; f.masked = mask;
  return encodeFrame(f);
}

inline std::vector<std::uint8_t> encodeText(const std::string& text, bool mask = false) {
  Frame f; f.opcode = Opcode::Text; f.payload.assign(text.begin(), text.end()); f.masked = mask;
  return encodeFrame(f);
}

} // namespace crashcore::ws
