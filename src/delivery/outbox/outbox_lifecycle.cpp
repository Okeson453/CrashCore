#include "delivery/outbox/outbox_lifecycle.hpp"

namespace crashcore {

const char* kOutboxLifecycleModule = "crashcore.delivery.outbox_lifecycle";

std::size_t outboxRecoverExpired(OutboxLifecycle& life) {
  return life.recoverExpiredLeases();
}

void outboxWake(OutboxLifecycle& life) {
  life.wake();
}

} // namespace crashcore
