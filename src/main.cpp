/**
 * CrashCore entry point.
 * Production binary: BC.Game ingestion → prediction interface → validation → outbox.
 * Prediction Engine remains a separate process/project (EXCLUDE from this binary).
 *
 * Source mapping: see include/audit/source_mapping.hpp
 */
#include "application/application.hpp"
#include "application/event_engine.hpp"
#include "audit/source_mapping.hpp"
#include "ingestion/socketio/protocol_edge_cases.hpp"
#include "logging/logger.hpp"
#include "config/toml_config.hpp"
#include <iostream>
#include <string>

int main(int argc, char** argv) {
  using namespace crashcore;

  ApplicationConfig cfg;
  std::string configPath;
  bool selfCheck = false;
  bool printAudit = false;

  for (int i = 1; i < argc; ++i) {
    std::string a = argv[i];
    if (a == "--debug") cfg.logLevel = "debug";
    else if (a == "--no-telegram") cfg.enableTelegram = false;
    else if (a == "--self-check") selfCheck = true;
    else if (a == "--audit") printAudit = true;
    else if (a == "--config" && i + 1 < argc) configPath = argv[++i];
  }

  if (cfg.logLevel == "debug") logger().setLevel(LogLevel::Debug);
  else if (cfg.logLevel == "trace") logger().setLevel(LogLevel::Trace);

  if (selfCheck) {
    int fails = runProtocolSelfCheck();
    if (fails) {
      std::cerr << "protocol self-check failed: " << fails << "\n";
      return 1;
    }
    std::cout << "protocol self-check OK\n";
    return 0;
  }

  if (printAudit) {
    printSourceAuditSummary();
    return 0;
  }

  Application app(std::move(cfg));
  if (!configPath.empty()) {
    auto r = app.loadConfigFile(configPath);
    if (!r) {
      std::cerr << "config load failed: " << r.error().message << "\n";
      return 1;
    }
  }
  return app.run();
}
