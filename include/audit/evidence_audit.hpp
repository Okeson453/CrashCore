#pragma once
#include <fstream>
#include <functional>
#include <string>
#include <vector>
#include <cstdio>

namespace crashcore::audit {

struct EvidenceEntry {
  std::string component, teReference, crashcorePath;
  std::size_t minLines = 0;
  std::string requiredSymbol;
  bool critical = false;
  std::function<bool()> checker;
};

inline bool fileHasMinLines(const std::string& path, std::size_t minLines) {
  std::ifstream in(path); if (!in) return false;
  std::size_t n = 0; std::string line;
  while (std::getline(in, line)) ++n;
  return n >= minLines;
}
inline bool fileContains(const std::string& path, const std::string& symbol) {
  if (symbol.empty()) return true;
  std::ifstream in(path); if (!in) return false;
  std::string content((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
  return content.find(symbol) != std::string::npos;
}

inline std::vector<EvidenceEntry> buildEvidenceRegistry(const std::string& root) {
  std::vector<EvidenceEntry> reg;
  auto add = [&](std::string comp, std::string te, std::string path, std::size_t ml, std::string sym, bool crit=false) {
    EvidenceEntry e; e.component=comp; e.teReference=te; e.crashcorePath=root+"/"+path;
    e.minLines=ml; e.requiredSymbol=sym; e.critical=crit;
    const auto p=e.crashcorePath; const auto m=e.minLines; const auto s=e.requiredSymbol;
    e.checker = [p,m,s]{ return fileHasMinLines(p,m) && fileContains(p,s); };
    reg.push_back(std::move(e));
  };
  add("mapEventName","normalizer.ts","include/ingestion/event_decoder.hpp",50,"st\"",true);
  add("BeastWebSocket","native-socket-client.ts","include/ingestion/websocket/beast_connection.hpp",100,"BeastWebSocketConnection",true);
  add("CurlHttpClient","fetch()","include/delivery/http/curl_http_client.hpp",50,"CurlHttpClient",true);
  add("PgDatabase","db.ts","include/persistence/pg/pg_database.hpp",50,"PgDatabase",true);
  add("CrashHistoryClient","fetch-bc.ts","include/ingestion/rest/crash_history_client.hpp",50,"fetchPage",true);
  add("PollWorker","poll-worker.ts","include/ingestion/rest/poll_worker.hpp",50,"PollWorker",true);
  add("ExternalSigner","native-sign.ts","include/ingestion/signing/external_signer_client.hpp",50,"ExternalSignerClient",true);
  add("OutboxRepository","migrations/outbox","include/persistence/outbox_repository.hpp",50,"SKIP LOCKED",true);
  add("NotificationWorker","notification-worker.ts","include/delivery/notification_worker.hpp",50,"NotificationWorker",true);
  add("LiveSupervisor","live-supervisor.ts","include/live/live_supervisor.hpp",50,"LiveSupervisor",true);
  add("LiveBoot","boot.ts","include/live/live_boot.hpp",50,"LiveBoot",true);
  add("ColdStartSeeder","cold-start-seeder.ts","include/live/cold_start_seeder.hpp",30,"ColdStartSeeder",false);
  add("ClockSkewMonitor","clock-skew-monitor.ts","include/live/clock_skew_monitor.hpp",30,"ClockSkewMonitor",false);
  add("RecentRoundCache","hot-cache.ts","include/observability/recent_round_cache.hpp",30,"RecentRoundCache",true);
  add("ReadinessBarrier","boot.ts","include/observability/readiness_barrier.hpp",50,"ReadinessBarrier",true);
  add("LifecycleMetrics","lifecycle-metrics.ts","include/observability/lifecycle_metrics.hpp",50,"recordLeadTimes",true);
  add("JsonLogger","logger.ts","include/logging/json_logger.hpp",50,"JsonLogger",false);
  add("DeliveryForensics","delivery-forensics.ts","include/feedback/delivery_forensics.hpp",40,"DeliveryForensics",false);
  add("MetricRegistryAtomic","metrics/registry.ts","include/telemetry/metric_registry.hpp",50,"AtomicCounter",true);
  return reg;
}

struct AuditReport { int passed=0, failed=0, criticalFailed=0; std::vector<std::string> failures; };

inline AuditReport runEvidenceAudit(const std::string& root = ".") {
  AuditReport report;
  for (auto& e : buildEvidenceRegistry(root)) {
    if (e.checker && e.checker()) ++report.passed;
    else {
      ++report.failed; if (e.critical) ++report.criticalFailed;
      report.failures.push_back(e.component + " [" + e.crashcorePath + "]");
    }
  }
  return report;
}

inline void printEvidenceAuditSummary(const std::string& root = ".") {
  auto r = runEvidenceAudit(root);
  std::printf("Evidence audit: %d passed, %d failed (%d critical)\n", r.passed, r.failed, r.criticalFailed);
  for (const auto& f : r.failures) std::printf("  FAIL: %s\n", f.c_str());
}

} // namespace crashcore::audit
