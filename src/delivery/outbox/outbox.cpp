#include "delivery/outbox/outbox.hpp"

namespace crashcore {

const char* kOutboxModule = "crashcore.delivery.outbox";

std::size_t outboxPendingSize(const Outbox& o) {
  return o.pendingSize();
}

Outbox::Stats outboxStats(const Outbox& o) {
  return o.stats();
}

} // namespace crashcore
