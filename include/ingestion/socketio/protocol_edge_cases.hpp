#pragma once
/**
 * PORT_PROTOCOL edge cases from TestingEngine native-protocol + transport audit.
 * Malformed frames, truncated varints, oversized nsp, mixed text/binary.
 */
#include "ingestion/socketio/socketio_protocol.hpp"
#include "ingestion/socketio/engineio.hpp"
#include "common/result.hpp"
#include <string>
#include <vector>
#include <optional>

namespace crashcore {

enum class FrameClass : std::uint8_t {
  Empty = 0,
  EnginePing,
  EnginePong,
  EngineOpen,
  EngineClose,
  EngineMessageText,
  BinaryPacket,
  Malformed,
  Truncated,
  Unknown
};

inline FrameClass classifyFrame(const std::uint8_t* data, std::size_t len) {
  if (len == 0) return FrameClass::Empty;
  if (engineio::isPing(data, len)) return FrameClass::EnginePing;
  if (engineio::isPong(data, len)) return FrameClass::EnginePong;
  if (engineio::isOpenFrame(data, len)) return FrameClass::EngineOpen;
  if (engineio::isClose(data, len)) return FrameClass::EngineClose;
  if (engineio::isMessage(data, len)) return FrameClass::EngineMessageText;
  if (data[0] == 0x04) {
    if (len < 2) return FrameClass::Truncated;
    // try parse
    auto p = parsePacket(data, len);
    if (p.kind == PacketKind::Unknown && p.event.empty() && p.nsp.empty())
      return FrameClass::Malformed;
    return FrameClass::BinaryPacket;
  }
  return FrameClass::Unknown;
}

inline bool isTrackedCrashEvent(std::string_view event) {
  return event == "pr" || event == "pg" || event == "bg" || event == "ed" ||
         event == "st" || event == "prepare" || event == "begin" || event == "end" ||
         event == "crash" || event == "bust" || event == "start" || event == "ready";
}

/** Validate nsp length and printable ASCII for BC.Game binary packets. */
inline bool validateNsp(std::string_view nsp) {
  if (nsp.empty() || nsp.size() > 64) return false;
  if (nsp[0] != '/') return false;
  for (char c : nsp) {
    if (c < 32 || c > 126) return false;
  }
  return true;
}

/** Safe decode of progress that never throws; returns nullopt on garbage. */
inline std::optional<std::int64_t> safeDecodeProgress(const std::uint8_t* data, std::size_t len) {
  if (!data || len == 0 || len > 64) return std::nullopt;
  try {
    return decodeProgressElapsed(data, len);
  } catch (...) {
    return std::nullopt;
  }
}

/** Build a sequence of synthetic frames for a full round (tests/replay). */
inline std::vector<std::vector<std::uint8_t>> syntheticRoundFrames(
    std::uint64_t gameId, double finalMult, int progressTicks = 10) {
  std::vector<std::vector<std::uint8_t>> frames;
  frames.push_back(encodeEvent("/g/cm", "st", encodeEndPayload(gameId, 1.0)));
  const auto totalElapsed = elapsedFromMultiplier(finalMult);
  for (int i = 1; i <= progressTicks; ++i) {
    auto elapsed = totalElapsed * i / progressTicks;
    frames.push_back(encodeEvent("/g/cm", "pg", encodeProgressPayload(elapsed)));
  }
  frames.push_back(encodeEvent("/g/cm", "ed", encodeEndPayload(gameId, finalMult, "synth")));
  return frames;
}

/** Stress-test varint roundtrip for values used on hot path. */
inline bool varintRoundtripOk(std::uint64_t value) {
  std::vector<std::uint8_t> buf;
  writeVarint(value, buf);
  std::size_t off = 0;
  std::uint64_t out = 0;
  if (!readVarint(buf.data(), buf.size(), off, out)) return false;
  return out == value && off == buf.size();
}

inline int runProtocolSelfCheck() {
  int fails = 0;
  if (!varintRoundtripOk(0)) ++fails;
  if (!varintRoundtripOk(127)) ++fails;
  if (!varintRoundtripOk(128)) ++fails;
  if (!varintRoundtripOk(300)) ++fails;
  if (!varintRoundtripOk(1'000'000)) ++fails;
  auto frames = syntheticRoundFrames(1, 2.5, 5);
  if (frames.size() != 7) ++fails;
  for (const auto& f : frames) {
    auto c = classifyFrame(f.data(), f.size());
    if (c != FrameClass::BinaryPacket) ++fails;
  }
  std::uint8_t ping = 0x32;
  if (classifyFrame(&ping, 1) != FrameClass::EnginePing) ++fails;
  return fails;
}

} // namespace crashcore
