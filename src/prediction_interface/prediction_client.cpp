#include "prediction_interface/prediction_client.hpp"
#include "prediction_interface/in_process_prediction_client.hpp"

namespace crashcore {

/**
 * Factory for PredictionClient.
 * Production default is in-process PE (InProcessPredictionClient).
 * Pass inProcess=false only for unit tests that intentionally exercise the
 * non-production stub or an external SHM/Unix client.
 */
std::unique_ptr<PredictionClient> makePredictionClient(bool inProcess = true) {
  if (inProcess) {
    return std::make_unique<InProcessPredictionClient>();
  }
  return std::make_unique<PredictionClient>();
}

} // namespace crashcore
