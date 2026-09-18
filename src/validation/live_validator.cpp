#include "validation/live_validator.hpp"
namespace crashcore {
const char* validateKindToString(ValidateKind k) {
  switch (k) {
    case ValidateKind::Matched: return "matched";
    case ValidateKind::NoPending: return "no_pending";
    case ValidateKind::BgArrivedLate: return "bg_arrived_late";
    case ValidateKind::DuplicateEnd: return "duplicate_end";
    case ValidateKind::Invalid: return "invalid";
    default: return "unknown";
  }
}
} // namespace crashcore
