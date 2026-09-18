#include "ingestion/websocket/websocket_client.hpp"
#include "ingestion/websocket/beast_connection.hpp"
namespace crashcore {
// start() -> makeWebSocketConnection(..., useBeast=true) -> Beast TLS + SNI.
// Query string includes EIO=3&transport=websocket and optional signed p/t.
} // namespace crashcore
