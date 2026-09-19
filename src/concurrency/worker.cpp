#include "concurrency/worker.hpp"

namespace crashcore {

const char* kWorkerModule = "crashcore.concurrency.worker";

void workerStart(Worker& w) {
  w.start();
}

void workerStop(Worker& w) {
  w.stop();
}

} // namespace crashcore
