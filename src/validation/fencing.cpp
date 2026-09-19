#include "validation/fencing.hpp"

namespace crashcore {

const char* kFencingModule = "crashcore.validation.fencing";

bool generationFenceAllows(const GenerationFence& fence,
                           const std::string& key,
                           std::uint64_t generation) {
  return fence.isCurrent(key, generation);
}

std::uint64_t globalEpochBump(GlobalEpoch& epoch) {
  return epoch.bump();
}

} // namespace crashcore
