#include "logging/logger.hpp"

namespace crashcore {

const char* kLoggerModule = "crashcore.logging.logger";

void logInfoApp(std::string_view msg) {
  logger().info(ComponentId::Application, msg);
}

void logWarnApp(std::string_view msg) {
  logger().warn(ComponentId::Application, msg);
}

void logErrorApp(std::string_view msg) {
  logger().error(ComponentId::Application, msg);
}

} // namespace crashcore
