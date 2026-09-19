#include "concurrency/worker_pool.hpp"

namespace crashcore {

const char* kWorkerPoolModule = "crashcore.concurrency.worker_pool";

void workerPoolStart(WorkerPool& p) {
  p.start();
}

void workerPoolStop(WorkerPool& p) {
  p.stop();
}

std::size_t workerPoolSize(const WorkerPool& p) {
  return p.worker_count();
}

std::size_t workerPoolQueueSize(const WorkerPool& p) {
  return p.queue_size();
}

} // namespace crashcore
