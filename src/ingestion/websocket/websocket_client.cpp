#include "ingestion/websocket/websocket_client.hpp"

namespace crashcore {

const char* kWebSocketClientModule = "crashcore.ingestion.websocket_client";

Result<void> webSocketClientStart(WebSocketClient& c) {
  return c.start();
}

void webSocketClientStop(WebSocketClient& c) {
  c.stop();
}

bool webSocketClientRunning(const WebSocketClient& c) {
  return c.running();
}

} // namespace crashcore
