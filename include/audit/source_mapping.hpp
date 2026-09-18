#pragma once
#include <cstdio>
/**
 * Source-of-truth audit: TestingEngine → CrashCore classification.
 * Classifications: PORT_DIRECTLY, PORT_BEHAVIOR, PORT_PROTOCOL, PORT_CONTRACT_ONLY,
 * REFERENCE_ONLY, EXCLUDE
 */
#include <string_view>
#include <array>

namespace crashcore::audit {

struct Mapping {
  std::string_view testingEnginePath;
  std::string_view crashCorePath;
  std::string_view classification;
  std::string_view status; // implemented | partial | pending
};

inline constexpr std::array<Mapping, 48> kSourceMap{{
  {"src/lib/crash/types.ts", "include/common/types.hpp", "PORT_CONTRACT", "implemented"},
  {"src/lib/crash/native-protocol.ts", "include/ingestion/socketio/socketio_protocol.hpp", "PORT_PROTOCOL", "implemented"},
  {"src/lib/crash/native-protocol.ts", "include/ingestion/socketio/protocol_edge_cases.hpp", "PORT_PROTOCOL", "implemented"},
  {"src/lib/crash/native-sign.ts", "include/ingestion/signing/native_sign.hpp", "PORT_PROTOCOL", "partial"},
  {"src/lib/crash/native-socket-client.ts", "include/ingestion/native_bc_socket.hpp", "PORT_BEHAVIOR", "implemented"},
  {"src/lib/crash/socket-client.ts", "include/ingestion/socketio/socketio_client.hpp", "PORT_BEHAVIOR", "implemented"},
  {"src/lib/crash/socket-diagnostics.ts", "include/ingestion/socket_diagnostics.hpp", "PORT_BEHAVIOR", "implemented"},
  {"src/lib/crash/transport/bcgame-crash-transport.ts", "include/ingestion/native_transport.hpp", "PORT_BEHAVIOR", "partial"},
  {"src/lib/crash/ingest.ts", "include/ingestion/crash_feed.hpp", "PORT_BEHAVIOR", "implemented"},
  {"src/lib/realtime/types.ts", "include/realtime/realtime_types.hpp", "PORT_CONTRACT", "implemented"},
  {"src/lib/realtime/normalizer.ts", "include/realtime/normalizer.hpp", "PORT_BEHAVIOR", "implemented"},
  {"src/lib/realtime/validator.ts", "include/realtime/event_validator.hpp", "PORT_BEHAVIOR", "implemented"},
  {"src/lib/realtime/realtime-pipeline.ts", "include/realtime/realtime_pipeline.hpp", "PORT_BEHAVIOR", "implemented"},
  {"src/lib/realtime/metrics.ts", "include/realtime/realtime_metrics.hpp", "PORT_BEHAVIOR", "implemented"},
  {"src/lib/prediction/types.ts", "include/prediction_interface/*.hpp", "PORT_CONTRACT_ONLY", "implemented"},
  {"src/lib/prediction/prediction-artifact.ts", "include/prediction_interface/prediction_event.hpp", "PORT_CONTRACT_ONLY", "implemented"},
  {"src/lib/prediction/live/prediction-attempt.ts", "include/prediction_interface/prediction_attempt.hpp", "PORT_CONTRACT", "implemented"},
  {"src/lib/prediction/live/validator.ts", "include/validation/live_validator.hpp", "PORT_BEHAVIOR", "implemented"},
  {"src/lib/prediction/live/feedback.ts", "include/validation/feedback_*.hpp", "PORT_CONTRACT", "implemented"},
  {"src/lib/prediction/live/live-round-state.ts", "include/ingestion/round_state.hpp", "PORT_BEHAVIOR", "implemented"},
  {"src/lib/prediction/live/fencing.ts", "include/validation/fencing.hpp", "PORT_BEHAVIOR", "implemented"},
  {"src/lib/prediction/live/clock-offset.ts", "include/timing/clock_offset.hpp", "PORT_BEHAVIOR", "implemented"},
  {"src/lib/prediction/identity/prediction-registry.ts", "include/validation/prediction_registry.hpp", "PORT_BEHAVIOR", "implemented"},
  {"src/lib/notifications/telegram.ts", "include/delivery/telegram/*", "PORT_BEHAVIOR", "partial"},
  {"src/lib/prediction/live/outbox-*.ts", "include/delivery/outbox/*", "PORT_BEHAVIOR", "implemented"},
  {"migrations/*outbox*", "include/persistence/schema.hpp", "PORT_BEHAVIOR", "implemented"},
  {"src/lib/db.ts", "include/persistence/database.hpp", "PORT_BEHAVIOR", "implemented"},
  {"src/lib/persistence/repositories/*", "include/persistence/repositories/*", "PORT_BEHAVIOR", "implemented"},
  {"src/lib/observability/logger.ts", "include/logging/*", "PORT_BEHAVIOR", "implemented"},
  {"src/lib/health.ts", "include/telemetry/health*.hpp", "PORT_BEHAVIOR", "implemented"},
  {"src/lib/prediction/prediction-engine.ts", "—", "EXCLUDE", "n/a"},
  {"src/lib/prediction/acie/*", "—", "EXCLUDE", "n/a"},
  {"src/lib/prediction/features/*", "—", "EXCLUDE", "n/a"},
  {"src/lib/prediction/models/*", "—", "EXCLUDE", "n/a"},
  {"src/lib/prediction/ensemble/*", "—", "EXCLUDE", "n/a"},
  {"src/lib/prediction/live/predictor.ts", "—", "REFERENCE_ONLY", "n/a"},
  {"src/lib/prediction/entry-decision-service.ts", "—", "REFERENCE_ONLY", "n/a"},
  {"src/lib/crash/fetch-bc.ts", "—", "REFERENCE_ONLY", "optional recovery"},
  {"src/lib/crash/api.ts", "—", "REFERENCE_ONLY", "n/a"},
  {"src/lib/crash/stats.ts", "include/ingestion/stats.hpp", "PORT_BEHAVIOR", "implemented"},
  {"src/lib/prediction/live/latency-trace.ts", "include/timing/latency_tracker.hpp", "PORT_BEHAVIOR", "implemented"},
  {"src/lib/prediction/live/latency-budgets.ts", "include/common/constants.hpp", "PORT_CONTRACT", "implemented"},
  {"src/lib/prediction/live/target-coordinator.ts", "—", "REFERENCE_ONLY", "PE-side"},
  {"src/lib/prediction/live/notification-worker.ts", "include/delivery/outbox/outbox_worker.hpp", "PORT_BEHAVIOR", "implemented"},
  {"src/lib/prediction/live/outbox-wake.ts", "include/delivery/outbox/outbox_lifecycle.hpp", "PORT_BEHAVIOR", "implemented"},
  {"src/lib/prediction/live/boot.ts", "include/live/live_boot.hpp", "PORT_BEHAVIOR", "implemented"},
  {"src/lib/prediction/live/live-supervisor.ts", "include/live/live_supervisor.hpp", "PORT_BEHAVIOR", "implemented"},
  {"src/lib/observability/event-loop-lag.ts", "include/telemetry/event_loop_lag.hpp", "PORT_BEHAVIOR", "implemented"},
}};

inline constexpr std::size_t mappingCount() { return kSourceMap.size(); }

} // namespace crashcore::audit

namespace crashcore {
inline void printSourceAuditSummary() {
  std::printf("CrashCore source audit map: %zu entries\n", audit::mappingCount());
  for (const auto& e : audit::kSourceMap) {
    std::printf("  [%s] %s -> %s (%s)\n", e.status.data(), e.testingEnginePath.data(), e.crashCorePath.data(), e.classification.data());
  }
}
}
