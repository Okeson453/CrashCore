#pragma once
#include <cstdint>
#include <string>
#include <vector>

namespace crashcore {

enum class PacketKind : std::uint8_t {
  Unknown = 0,
  Connect,
  Disconnect,
  Event,
  Ack,
  Error,
  BinaryEvent,
  BinaryAck,
  Ping,
  Pong,
  Open,
  Other
};

struct SocketIoPacket {
  PacketKind              kind = PacketKind::Unknown;
  std::string             nsp;
  std::string             event;
  std::vector<std::uint8_t> payload;
  std::string             sid;          // session id from open
  int                     pingInterval = 0;
  int                     pingTimeout  = 0;
};

} // namespace crashcore
