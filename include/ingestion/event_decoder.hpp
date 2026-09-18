#pragma once
/**
 * Event decoder — maps raw Socket.IO / native frames to CrashEvent.
 * Behaviour from TestingEngine realtime/normalizer.ts + native-protocol.ts.
 */
#include "ingestion/crash_event.hpp"
#include "ingestion/socketio/socketio_protocol.hpp"
#include "ingestion/socketio/socketio_packet.hpp"
#include "ingestion/socketio/engineio.hpp"
#include "timing/latency_tracker.hpp"
#include "timing/timestamp.hpp"
#include "common/result.hpp"
#include <string>
#include <string_view>
#include <optional>
#include <atomic>

namespace crashcore {

class EventDecoder {
public:
  explicit EventDecoder(LatencyTracker* tracker = nullptr) : tracker_(tracker) {}

  /** Decode a binary Engine.IO / Socket.IO frame into zero or more CrashEvents. */
  Result<CrashEvent> decodeBinary(const std::uint8_t* data, std::size_t len,
                                  EventSource source = EventSource::NativeSocket) {
    const auto t0 = steadyNs();
    if (len == 0) return Error{ErrorCode::ParseError, "empty frame"};

    // Engine.IO control frames
    if (engineio::isPing(data, len)) {
      CrashEvent ev;
      ev.kind = EventKind::Heartbeat;
      ev.source = source;
      ev.receivedAtNs = t0;
      ev.decodedAtNs = steadyNs();
      ev.valid = true;
      ev.rawEventName = "ping";
      return ev;
    }
    if (engineio::isPong(data, len) || engineio::isOpenFrame(data, len) || engineio::isClose(data, len)) {
      return Error{ErrorCode::ParseError, "control frame not an event"};
    }

    // Custom BC.Game binary packet (0x04 ...)
    if (data[0] == 0x04) {
      auto pkt = parsePacket(data, len);
      if (pkt.kind != PacketKind::Event) {
        if (pkt.kind == PacketKind::Connect) {
          CrashEvent ev;
          ev.kind = EventKind::Connect;
          ev.source = source;
          ev.receivedAtNs = t0;
          ev.decodedAtNs = steadyNs();
          ev.valid = true;
          ev.rawEventName = "connect";
          return ev;
        }
        return Error{ErrorCode::ParseError, "non-event packet"};
      }
      return decodeEventPacket(pkt, source, t0);
    }

    // Text Engine.IO message "4..."
    if (engineio::isMessage(data, len) && len > 1) {
      // Fallback: treat remaining as opaque — real text path uses JSON
      return Error{ErrorCode::ParseError, "text message path not used on hot path"};
    }

    return Error{ErrorCode::ParseError, "unrecognised frame"};
  }

  Result<CrashEvent> decodeEventPacket(const SocketIoPacket& pkt, EventSource source, NanoTime receivedNs) {
    CrashEvent ev;
    ev.source = source;
    ev.receivedAtNs = receivedNs;
    ev.rawEventName = pkt.event;
    ev.kind = mapEventName(pkt.event);
    if (ev.kind == EventKind::Unknown) {
      return Error{ErrorCode::ParseError, "unknown event name: " + pkt.event};
    }

    if (!pkt.payload.empty()) {
      if (ev.kind == EventKind::Progress) {
        auto elapsed = decodeProgressElapsed(pkt.payload.data(), pkt.payload.size());
        if (elapsed) {
          ev.elapsedMs = *elapsed;
          ev.currentMult = multiplierFromElapsed(*elapsed);
          ev.valid = true;
        }
      } else {
        auto fields = decodeProtobuf(pkt.payload.data(), pkt.payload.size());
        auto payload = fieldsToPayload(pkt.event, fields);
        if (payload.gameId) {
          ev.gameId = *payload.gameId;
          ev.roundId = *payload.gameId;
        }
        if (payload.multiplier) {
          if (ev.kind == EventKind::End) ev.crashPoint = *payload.multiplier;
          else ev.currentMult = *payload.multiplier;
        }
        if (payload.hash) ev.hash = *payload.hash;
        if (payload.beginTime) ev.beganAtMs = *payload.beginTime;
        if (payload.endTime) ev.endedAtMs = *payload.endTime;
        if (payload.elapsedMs) {
          ev.elapsedMs = *payload.elapsedMs;
          ev.currentMult = multiplierFromElapsed(*payload.elapsedMs);
        }
        ev.valid = !ev.gameId.empty() || ev.kind == EventKind::Progress;
      }
    } else {
      // Some events may have empty payload
      ev.valid = (ev.kind == EventKind::Connect || ev.kind == EventKind::Heartbeat);
    }

    ev.decodedAtNs = steadyNs();
    ev.eventTimeMs = nowMs();
    if (tracker_) {
      const auto us = (ev.decodedAtNs - receivedNs) / 1000;
      tracker_->record(LatencyTracker::Stage::Parse, us);
    }
    ++decoded_;
    if (!ev.valid) ++invalid_;
    return ev;
  }

  static EventKind mapEventName(std::string_view event) {
    // Matches TE normalizer.ts:45-55 — "st" is End, "bg" is Start/Begin
    std::string key(event);
    for (auto& c : key) if (c >= 'A' && c <= 'Z') c += 32;
    if (key == "pg" || key == "pr") return key == "pg" ? EventKind::Progress : EventKind::Prepare;
    if (key == "prepare" || key == "ready") return EventKind::Prepare;
    if (key == "bg" || key == "begin" || key == "start") return EventKind::Start;
    if (key == "ed" || key == "st" || key == "end" || key == "crash" || key == "bust")
      return EventKind::End;
    if (key == "ping" || key == "pong") return EventKind::Heartbeat;
    return EventKind::Unknown;
  }

  std::uint64_t decodedCount() const noexcept { return decoded_.load(std::memory_order_relaxed); }
  std::uint64_t invalidCount() const noexcept { return invalid_.load(std::memory_order_relaxed); }

private:
  LatencyTracker* tracker_ = nullptr;
  std::atomic<std::uint64_t> decoded_{0};
  std::atomic<std::uint64_t> invalid_{0};
};

} // namespace crashcore
