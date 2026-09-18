#pragma once
#include "common/types.hpp"
#include <string>
#include <cstddef>

namespace crashcore {

struct ApplicationConfig {
  std::string host = "bc.game";
  std::string path = "/game/crash";
  std::string nsp  = "/g/cm";
  std::size_t eventQueueCapacity = 4096;
  std::size_t workerCount = 4;
  std::size_t outboxCapacity = 2048;
  std::int64_t predictionTimeoutMs = 50;
  std::int64_t heartbeatIntervalMs = 25000;
  bool enableTelegram = true;
  bool enablePersistence = false; // when true, DB connect/migrate is required (hard-fail)
  std::string logLevel = "info";
  std::int64_t degradedMs = 15'000;
  std::int64_t staleMs = 90'000;
  std::int64_t signFreshMs = 60'000;
  std::int64_t signStaleMs = 600'000;
  std::int64_t stuckMaxAgeMs = 180'000;
  std::int64_t lossCooldownMs = 60'000;
  int lossCooldownThreshold = 3;
  bool enableNativeSocket = true;
  bool enablePollWorker = true;
  bool enableNotificationWorker = true;
  std::int64_t pollIntervalMs = 5000;
  bool enableRealtimePipeline = true;
  std::string socketHost = "socketv4.bc.game";
  // Phase closure config
  std::uint16_t statsPort = 8080;
  double qualityEdge = 0.02;
  double reducedEdge = 0.005;
  double minConfidence = 0.55;
  std::int64_t deliveryDeadlineMs = 5000;
  bool enableDurableOutbox = true;
  bool enableClockSkewMonitor = true;
  bool enableFunnelMetrics = true;
  bool enableDeliveryForensics = true;

};

} // namespace crashcore
