/**
 * Realtime pipeline translation unit — explicit instantiations and helpers.
 * Behaviour source: TestingEngine src/lib/realtime/*
 */
#include "realtime/realtime_pipeline.hpp"
#include "realtime/normalizer.hpp"
#include "realtime/event_validator.hpp"
#include "realtime/realtime_metrics.hpp"
#include "logging/structured_logger.hpp"
#include <sstream>

namespace crashcore::realtime {

std::string formatMetricsSnapshot(const MetricsSnapshot& m) {
  std::ostringstream oss;
  oss << "received=" << m.eventsReceived
      << " accepted=" << m.eventsAccepted
      << " dup=" << m.duplicates
      << " stale=" << m.stale
      << " invalid=" << m.invalid
      << " missed=" << m.missed
      << " reconnects=" << m.reconnects;
  if (m.lastArrivalLagMs) oss << " arrivalLagMs=" << *m.lastArrivalLagMs;
  if (m.avgProcessingMs) oss << " avgProcMs=" << *m.avgProcessingMs;
  if (m.lastE2eMs) oss << " e2eMs=" << *m.lastE2eMs;
  return oss.str();
}

void logRealtimeSnapshot(const RealtimePipeline& pipeline) {
  StructuredLogger log(ComponentId::Ingestion);
  log.info("realtime ", formatMetricsSnapshot(pipeline.metricsSnapshot()));
}

} // namespace crashcore::realtime
