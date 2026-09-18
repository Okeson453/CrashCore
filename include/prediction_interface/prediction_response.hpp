#pragma once
#include "common/types.hpp"

namespace crashcore {

// PredictionResponse is defined in common/types.hpp

inline bool isActionable(const PredictionResponse& r) noexcept {
  return r.valid && r.decision == PredictionDecision::Enter && r.confidence > 0.0;
}

} // namespace crashcore
