/**
 * Integration: EventEngine synthetic rounds + protocol fixtures + audit map.
 */
#include "application/event_engine.hpp"
#include "ingestion/test_fixtures.hpp"
#include "ingestion/socketio/protocol_edge_cases.hpp"
#include "audit/source_mapping.hpp"
#include "security/secret_provider.hpp"
#include "realtime/realtime_pipeline.hpp"
#include <cstdio>

using namespace crashcore;
static int fails = 0;
#define CHECK(c) do { if (!(c)) { std::printf("FAIL %s:%d %s\n", __FILE__, __LINE__, #c); ++fails; } } while(0)

int main() {
  CHECK(runProtocolSelfCheck() == 0);
  CHECK(audit::mappingCount() >= 40);
  CHECK(fixtures::countFixtures() == 80);

  MapSecretProvider sp({});
  EventEngine engine(sp);
  CHECK(engine.start());

  // 20 synthetic rounds
  for (int i = 0; i < 20; ++i) {
    CHECK(engine.processSyntheticRound(static_cast<std::uint64_t>(5000 + i), 1.5 + i * 0.05));
  }

  auto& st = engine.pipeline().stats();
  CHECK(st.count() >= 1);

  // Feed named fixtures subset
  auto frames = fixtures::fixture_round_001();
  for (const auto& f : frames) {
    engine.processFrame(f.data(), f.size());
  }
  engine.pipeline().drain();

  engine.stop();

  // Realtime pipeline unit path
  realtime::RealtimePipeline rt;
  CrashEvent end;
  end.kind = EventKind::End;
  end.valid = true;
  end.gameId = "rt-1";
  end.roundId = "rt-1";
  end.crashPoint = 2.0;
  end.eventTimeMs = nowMs();
  end.receivedAtNs = steadyNs();
  auto acc = rt.observe(end);
  CHECK(acc.has_value());

  if (fails) {
    std::printf("%d failure(s)\n", fails);
    return 1;
  }
  std::printf("pipeline_engine_test OK\n");
  return 0;
}
