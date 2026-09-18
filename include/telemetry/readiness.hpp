#pragma once
/**
 * PORT_BEHAVIOR from TestingEngine observability/readiness.ts
 */
#include "telemetry/health.hpp"
#include "orchestration/component_registry.hpp"
#include "ingestion/native_bc_socket.hpp"
#include "delivery/outbox/outbox.hpp"
#include <string>
#include <sstream>

namespace crashcore {

struct ReadinessReport {
  bool ready = false;
  bool live = false;
  std::string reason;
  std::vector<std::string> checks;
};

class ReadinessChecker {
public:
  ReadinessReport check(const NativeBcGameSocket* socket,
                        const ComponentRegistry* registry,
                        const Outbox* outbox) const {
    ReadinessReport r;
    r.ready = true;
    r.live = true;

    if (socket) {
      auto st = socket->status();
      bool up = (st == NativeSocketStatus::Connected || st == NativeSocketStatus::Degraded);
      r.checks.push_back(std::string("socket=") + toString(st));
      if (!up) { r.ready = false; r.live = false; r.reason = "socket not connected"; }
    } else {
      r.checks.push_back("socket=absent");
    }

    if (registry) {
      r.checks.push_back("components=" + std::to_string(registry->count()));
      if (registry->count() == 0) {
        r.ready = false;
        r.reason = "no components registered";
      }
    }

    if (outbox) {
      r.checks.push_back("outbox_pending=" + std::to_string(outbox->pendingSize()));
      // outbox backlog does not fail readiness, only liveness soft signal
      if (outbox->pendingSize() > 10000) {
        r.live = false;
        if (r.reason.empty()) r.reason = "outbox backlog high";
      }
    }

    if (r.reason.empty()) r.reason = r.ready ? "ok" : "not ready";
    return r;
  }
};

} // namespace crashcore
