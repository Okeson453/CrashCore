#include "ingestion/event_router.hpp"
namespace crashcore {
// LRU dedupe (seen_map_) prevents unbounded growth of seen game ids.
// Routes Start/Progress to prediction queue, End to validation queue.
} // namespace crashcore
