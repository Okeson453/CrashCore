#include "application/application.hpp"
#include "api/statistics_server.hpp"
#include "ingestion/websocket/beast_connection.hpp"
#include "persistence/pg/pg_database.hpp"
namespace crashcore {
std::unique_ptr<HttpClient> createAppHttpClient() { return makeHttpClient(true); }
std::unique_ptr<Database> createAppDatabase(const DatabaseCredentials& c) { return makeDatabase(c, true); }
std::unique_ptr<WebSocketConnection> createAppWebSocket(WebSocketConfig cfg, WebSocketMetrics* m) {
  return makeWebSocketConnection(std::move(cfg), m, true);
}
} // namespace crashcore
