#include "orchestration/shutdown_manager.hpp"

namespace crashcore {

const char* kShutdownManagerModule = "crashcore.orchestration.shutdown_manager";

void runShutdownManager(ShutdownManager& sm) {
  sm.stopInReverseOrder();
}

} // namespace crashcore
