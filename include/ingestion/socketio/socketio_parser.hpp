#pragma once
/**
 * Higher-level Socket.IO packet stream parser.
 * Handles fragmented Engine.IO payloads and text JSON events when present.
 * Hot path prefers binary BC.Game frames via socketio_protocol.hpp.
 */
#include "ingestion/socketio/socketio_packet.hpp"
#include "ingestion/socketio/socketio_protocol.hpp"
#include "ingestion/socketio/engineio.hpp"
#include "common/result.hpp"
#include <string>
#include <string_view>
#include <vector>
#include <optional>

namespace crashcore {

class SocketIoParser {
public:
  /** Feed a complete WebSocket binary/text message. */
  Result<std::vector<SocketIoPacket>> feed(const std::uint8_t* data, std::size_t len) {
    std::vector<SocketIoPacket> out;
    if (len == 0) return out;

    if (engineio::isPing(data, len) || engineio::isPong(data, len) ||
        engineio::isClose(data, len)) {
      SocketIoPacket p;
      if (engineio::isPing(data, len)) p.kind = PacketKind::Ping;
      else if (engineio::isPong(data, len)) p.kind = PacketKind::Pong;
      else p.kind = PacketKind::Disconnect;
      out.push_back(std::move(p));
      return out;
    }

    if (engineio::isOpenFrame(data, len)) {
      out.push_back(engineio::parseOpen(data, len));
      return out;
    }

    // Engine.IO message type '4' followed by Socket.IO payload
    if (engineio::isMessage(data, len) && len > 1) {
      // Text path: "42/namespace,[\"event\",...]" — not used on BC hot path
      SocketIoPacket p;
      p.kind = PacketKind::Event;
      p.payload.assign(data + 1, data + len);
      out.push_back(std::move(p));
      return out;
    }

    if (data[0] == 0x04) {
      out.push_back(parsePacket(data, len));
      return out;
    }

    return Error{ErrorCode::ParseError, "unrecognised Socket.IO frame"};
  }

  Result<std::vector<SocketIoPacket>> feed(const std::vector<std::uint8_t>& buf) {
    return feed(buf.data(), buf.size());
  }
};

} // namespace crashcore
