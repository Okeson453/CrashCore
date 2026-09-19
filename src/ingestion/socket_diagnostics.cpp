#include "ingestion/socket_diagnostics.hpp"

namespace crashcore {

const char* kSocketDiagnosticsModule = "crashcore.ingestion.socket_diagnostics";

std::string socketDiagnosticsSummary(const SocketDiagnostics& d) {
  return d.summary();
}

} // namespace crashcore
