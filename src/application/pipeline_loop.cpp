#include "application/pipeline_loop.hpp"

namespace crashcore {

const char* kPipelineLoopModule = "crashcore.application.pipeline_loop";

/**
 * Standalone drain helper for tests that own a PipelineLoop without Application.
 * Production path: Application::drainQueues (N1Coordinator authority).
 */
void pipelineLoopDrainOnce(PipelineLoop& loop) {
  loop.drain();
}

} // namespace crashcore
