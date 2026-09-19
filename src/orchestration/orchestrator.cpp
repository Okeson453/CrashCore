#include "orchestration/orchestrator.hpp"

namespace crashcore {

const char* kOrchestratorModule = "crashcore.orchestration.orchestrator";

Result<void> orchestratorStart(Orchestrator& o) {
  return o.startAll();
}

void orchestratorStop(Orchestrator& o) {
  o.stopAll();
}

std::size_t orchestratorComponentCount(const Orchestrator& o) {
  return o.componentCount();
}

} // namespace crashcore
