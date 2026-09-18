/**
 * Run offline scenarios through EventEngine for validation.
 */
#include "application/event_engine.hpp"
#include "ingestion/scenarios.hpp"
#include "security/secret_provider.hpp"
#include <cstdio>

namespace crashcore {

int runScenarioBatch(int maxScenarios = 50) {
  MapSecretProvider sp({});
  EventEngine engine(sp);
  if (!engine.start()) {
    std::printf("engine start failed\n");
    return 1;
  }
  auto all = scenarios::allScenarios();
  int n = 0;
  int errors = 0;
  for (const auto& sc : all) {
    if (n >= maxScenarios) break;
    for (const auto& step : sc.steps) {
      auto r = engine.processFrame(step.frame.data(), step.frame.size());
      if (!r && r.error().code != ErrorCode::ParseError &&
          r.error().code != ErrorCode::Duplicate) {
        ++errors;
      }
    }
    engine.pipeline().drain();
    ++n;
  }
  engine.stop();
  std::printf("scenarios_run=%d errors=%d stats_rounds=%llu\n", n, errors,
              static_cast<unsigned long long>(engine.pipeline().stats().count()));
  return errors > 0 ? 1 : 0;
}

} // namespace crashcore
