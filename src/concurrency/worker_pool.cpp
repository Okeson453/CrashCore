#include "concurrency/worker_pool.hpp"
namespace crashcore {
// WorkerPool: N per-worker BoundedQueues + work-stealing victim scan.
// start() launches std::jthread workers; stop() closes queues and joins.
// Prefer try_submit on the least-loaded queue to reduce steals under steady load.
} // namespace crashcore
