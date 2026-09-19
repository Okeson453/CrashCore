#include "ingestion/crash_feed.hpp"

namespace crashcore {

const char* kCrashFeedModule = "crashcore.ingestion.crash_feed";

std::uint64_t crashFeedFrameCount(const CrashFeed& feed) {
  return feed.frameCount();
}

} // namespace crashcore
