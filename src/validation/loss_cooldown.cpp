#include "validation/loss_cooldown.hpp"

namespace crashcore {

const char* kLossCooldownModule = "crashcore.validation.loss_cooldown";

bool lossCooldownActive(const LossCooldownTracker& t, TimestampMs now) {
  return t.inCooldown(now);
}

} // namespace crashcore
