/**
 * End-to-end integration: synthetic frames → decode → route → predict → validate → outbox
 */
#include "application/application.hpp"
#include "ingestion/socketio/socketio_protocol.hpp"
#include "delivery/outbox/outbox.hpp"
#include <cstdio>
#include <cassert>

using namespace crashcore;

int main() {
  ApplicationConfig cfg;
  cfg.enableTelegram = false;
  Application app(cfg);
  assert(app.init());

  // Inject a full round
  auto startPayload = encodeEndPayload(99, 1.0);
  auto startFrame = encodeEvent("/g/cm", "st", startPayload);
  assert(app.onBinaryFrame(startFrame.data(), startFrame.size()));

  for (int i = 1; i <= 5; ++i) {
    auto frame = encodeEvent("/g/cm", "pg", encodeProgressPayload(i * 2000));
    assert(app.onBinaryFrame(frame.data(), frame.size()));
  }

  auto endFrame = encodeEvent("/g/cm", "ed", encodeEndPayload(99, 3.0, "hash99"));
  assert(app.onBinaryFrame(endFrame.data(), endFrame.size()));

  app.drainQueues();
  app.stop();

  std::printf("integration end_to_end OK\n");
  return 0;
}
