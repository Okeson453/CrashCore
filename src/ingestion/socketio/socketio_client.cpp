#include "ingestion/socketio/socketio_client.hpp"

namespace crashcore {

const char* kSocketIoClientModule = "crashcore.ingestion.socketio_client";

void socketIoOnBinaryFrame(SocketIoClient& c, const std::uint8_t* data, std::size_t len) {
  c.onBinaryFrame(data, len);
}

} // namespace crashcore
