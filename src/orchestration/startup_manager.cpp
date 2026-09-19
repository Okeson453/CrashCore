#include "orchestration/startup_manager.hpp"

namespace crashcore {

const char* kStartupManagerModule = "crashcore.orchestration.startup_manager";

Result<void> runStartupManager(StartupManager& sm) {
  return sm.startInOrder();
}

} // namespace crashcore
