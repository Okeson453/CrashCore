#include "api/statistics_server.hpp"

namespace crashcore {

const char* kStatisticsServerModule = "crashcore.api.statistics_server";

bool statisticsServerRunning(const StatisticsServer& s) {
  return s.running();
}

} // namespace crashcore
