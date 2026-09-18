#include "delivery/outbox/outbox.hpp"
namespace crashcore {
// In-memory Outbox provides sub-ms publish/claim for the hot path.
// Durable path: OutboxRepository + PgDatabase (FOR UPDATE SKIP LOCKED).
// Application wires DurableHandoff -> OutboxWorker for async Telegram delivery.
} // namespace crashcore
