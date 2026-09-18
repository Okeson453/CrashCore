#include "ingestion/event_decoder.hpp"
namespace crashcore {
// Public helper mirroring EventDecoder::mapEventName for tests/tools.
EventKind publicMapEventName(std::string_view event) {
  std::string key(event);
  for (auto& c : key) if (c >= 'A' && c <= 'Z') c += 32;
  if (key == "pg" || key == "pr") return key == "pg" ? EventKind::Progress : EventKind::Prepare;
  if (key == "prepare" || key == "ready") return EventKind::Prepare;
  if (key == "bg" || key == "begin" || key == "start") return EventKind::Start;
  if (key == "ed" || key == "st" || key == "end" || key == "crash" || key == "bust")
    return EventKind::End;
  if (key == "ping" || key == "pong") return EventKind::Heartbeat;
  return EventKind::Unknown;
}
} // namespace crashcore
