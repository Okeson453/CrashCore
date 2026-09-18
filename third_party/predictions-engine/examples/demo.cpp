#include "pe/engine/prediction_engine.hpp"

#include <iostream>
#include <random>

int main() {
  pe::PredictionEngine engine;

  std::mt19937 rng{42};
  std::uniform_real_distribution<double> dist(1.0, 4.0);

  std::cout << "Seeding IncrementalStateEngine with 120 synthetic crashes...\n";
  for (int i = 0; i < 120; ++i) {
    const double cp = dist(rng);
    engine.observe(cp, 1'700'000'000'000LL + i * 22'000);
  }

  std::cout << "Lifecycle: " << pe::toString(engine.state().getLifecycleState()) << "\n";
  std::cout << "Hit rate ≥1.3: " << engine.state().hitRate(1.3) << "\n";

  pe::PredictRequest req;
  req.targetRoundId = "demo-round-001";
  req.target = 1.3;

  try {
    auto result = engine.predict(req);
    std::cout << "\n=== Prediction Signal ===\n";
    std::cout << "  id:          " << result.signal.predictionId << "\n";
    std::cout << "  probability: " << result.signal.probability << "\n";
    std::cout << "  confidence:  " << result.signal.confidence << "\n";
    std::cout << "  regime:      " << result.regime.id << "\n";
    std::cout << "  model:       " << result.signal.modelVersion << "\n";
    std::cout << "  features:    " << result.features.values.size() << " keys\n";
    std::cout << "  reasoning:\n";
    for (const auto& r : result.signal.reasoning)
      std::cout << "    - " << r << "\n";
  } catch (const pe::PipelineStageError& e) {
    std::cerr << "Pipeline error [" << pe::toString(e.stage) << "]: " << e.what() << "\n";
    return 1;
  }

  return 0;
}
