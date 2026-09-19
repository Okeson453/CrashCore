#include "ingestion/websocket/websocket_session.hpp"

namespace crashcore {

const char* kWebSocketSessionModule = "crashcore.ingestion.websocket_session";

Result<void> webSocketSessionStart(WebSocketSession& s) {
  return s.start();
}

void webSocketSessionStop(WebSocketSession& s) {
  s.stop();
}

} // namespace crashcore
