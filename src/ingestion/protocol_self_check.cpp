#include "ingestion/socketio/protocol_edge_cases.hpp"
#include <cstdio>

namespace crashcore {

int protocol_self_check_main() {
  int fails = runProtocolSelfCheck();
  if (fails == 0) {
    std::printf("protocol self-check OK\n");
    return 0;
  }
  std::printf("protocol self-check FAIL count=%d\n", fails);
  return 1;
}

} // namespace crashcore
