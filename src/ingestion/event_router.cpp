#include "ingestion/event_router.hpp"

namespace crashcore {

const char* kEventRouterModule = "crashcore.ingestion.event_router";

EventRouter::Stats eventRouterStats(const EventRouter& r) {
  return r.stats();
}

} // namespace crashcore
