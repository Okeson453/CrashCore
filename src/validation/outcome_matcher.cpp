#include "validation/outcome_matcher.hpp"

namespace crashcore {

const char* kOutcomeMatcherModule = "crashcore.validation.outcome_matcher";

std::vector<Outcome> matchOutcomesPublic(OutcomeMatcher& m, const CrashEvent& endEvent) {
  return m.match(endEvent);
}

std::uint64_t outcomeMatcherMatchedCount(const OutcomeMatcher& m) {
  return m.matchedCount();
}

} // namespace crashcore
