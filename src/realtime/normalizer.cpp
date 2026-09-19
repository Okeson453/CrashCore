#include "realtime/normalizer.hpp"

namespace crashcore::realtime {

const char* kNormalizerModule = "crashcore.realtime.normalizer";

std::optional<RoundPhase> mapEventNamePublic(std::string event) {
  return mapEventName(std::move(event));
}

std::optional<NormalizedRoundEvent> normalizeSourceEventPublic(const RawSourceEvent& raw) {
  return normalizeSourceEvent(raw);
}

} // namespace crashcore::realtime
