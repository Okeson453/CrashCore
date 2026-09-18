#include "prediction_interface/prediction_client.hpp"
#include "prediction_interface/in_process_prediction_client.hpp"
namespace crashcore {
std::unique_ptr<PredictionClient> makePredictionClient(bool inProcess) {
  if (inProcess) return std::make_unique<InProcessPredictionClient>();
  return std::make_unique<PredictionClient>();
}
} // namespace crashcore
