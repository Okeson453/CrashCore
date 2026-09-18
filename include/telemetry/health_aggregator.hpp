#pragma once
/**
 * Aggregates connection, queue, worker, and component health into SystemHealth.
 */
#include "telemetry/health.hpp"
#include "telemetry/metrics.hpp"
#include "common/types.hpp"
#include "ingestion/native_bc_socket.hpp"
#include "concurrency/worker_pool.hpp"
#include "delivery/outbox/outbox.hpp"
#include "orchestration/component_registry.hpp"
#include <sstream>
#include <string>

namespace crashcore {

class HealthAggregator {
public:
  SystemHealth collect(
      const NativeBcGameSocket* socket,
      const WorkerPool* workers,
      const Outbox* outbox,
      const ComponentRegistry* registry,
      const Metrics* metrics = nullptr) const {
    SystemHealth h;
    h.healthy = true;

    if (socket) {
      h.connection = socket->health();
      if (!h.connection.connected) h.healthy = false;
    }

    if (workers) {
      h.predictionQueue = workers->queue_health();
      // Synthesize worker health entries from pool size
      h.workers.clear();
      for (std::size_t i = 0; i < workers->worker_count(); ++i) {
        WorkerHealth wh;
        wh.running = true;
        wh.name = "worker-" + std::to_string(i);
        h.workers.push_back(wh);
      }
    }

    if (outbox) {
      h.outboxQueue.size = outbox->pendingSize();
      auto st = outbox->stats();
      h.outboxQueue.pushes = st.published;
      h.outboxQueue.pops = st.delivered;
      h.outboxQueue.drops = st.failed + st.deadLetters;
    }

    std::ostringstream oss;
    oss << (h.healthy ? "healthy" : "degraded");
    if (socket) {
      oss << " socket=" << (h.connection.connected ? "up" : "down");
      oss << " reconnects=" << h.connection.reconnects;
    }
    if (outbox) {
      oss << " outbox_pending=" << h.outboxQueue.size;
    }
    if (metrics) {
      oss << " events=" << metrics->get(Metrics::EventsReceived);
      oss << " wins=" << metrics->get(Metrics::OutcomesWin);
      oss << " losses=" << metrics->get(Metrics::OutcomesLoss);
    }
    if (registry) {
      oss << " components=" << registry->count();
    }
    h.summary = oss.str();
    return h;
  }
};

} // namespace crashcore
