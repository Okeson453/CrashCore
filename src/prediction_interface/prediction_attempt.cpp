#include "prediction_interface/prediction_attempt.hpp"

namespace crashcore {

const char* kPredictionAttemptModule = "crashcore.prediction_interface.prediction_attempt";

std::size_t predictionAttemptExpire(PredictionAttemptCoordinator& c) {
  return c.expireTimeouts();
}

} // namespace crashcore
