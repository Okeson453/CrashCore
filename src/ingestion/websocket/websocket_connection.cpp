#include "ingestion/websocket/websocket_connection.hpp"
#include "ingestion/websocket/beast_connection.hpp"

namespace crashcore {

const char* kWebSocketConnectionModule = "crashcore.ingestion.websocket_connection";

std::unique_ptr<WebSocketConnection> createProductionWebSocket(WebSocketConfig cfg,
                                                               WebSocketMetrics* metrics) {
  return makeWebSocketConnection(std::move(cfg), metrics, true);
}

} // namespace crashcore
