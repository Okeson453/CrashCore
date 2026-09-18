#pragma once
#include "common/types.hpp"
#include <vector>
#include <string>

namespace crashcore {

struct SystemHealth {
  bool healthy = true;
  ConnectionHealth connection;
  QueueHealth predictionQueue;
  QueueHealth validationQueue;
  QueueHealth outboxQueue;
  std::vector<WorkerHealth> workers;
  std::string summary;
};

} // namespace crashcore
