#include "ingestion/websocket/websocket_connection.hpp"
#include "ingestion/websocket/beast_connection.hpp"
namespace crashcore {
std::unique_ptr<WebSocketConnection> createDefaultWebSocket(WebSocketConfig cfg, WebSocketMetrics* m) {
  return makeWebSocketConnection(std::move(cfg), m, true);
}
} // namespace crashcore
