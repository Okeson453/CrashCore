#include "validation/prediction_registry.hpp"

namespace crashcore {

const char* kPredictionRegistryModule = "crashcore.validation.prediction_registry";

std::size_t predictionRegistrySize(const PredictionRegistry& reg) {
  return reg.size();
}

} // namespace crashcore
