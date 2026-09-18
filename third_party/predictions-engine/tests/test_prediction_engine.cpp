#include "pe/engine/prediction_engine.hpp"
#include <iostream>
#include <string>

static int failures = 0;
#define CHECK(c) do { if (!(c)) { std::cerr << "FAIL " << #c << " @" << __LINE__ << "\n"; ++failures; } } while (0)

int main() {
  using namespace pe;

  {
    PredictionEngine engine;
    for (int i = 0; i < 60; ++i)
      engine.observe(1.0 + (i % 4) * 0.4, 1'700'000'000'000LL + i * 25'000);

    PredictRequest req;
    req.targetRoundId = "round-test-1";
    req.target = 1.3;
    auto result = engine.predict(req);

    CHECK(result.signal.targetRoundId == "round-test-1");
    CHECK(result.signal.probability >= 0.01 && result.signal.probability <= 0.99);
    CHECK(result.signal.featurePath == FeaturePath::V2_INCREMENTAL);
    CHECK(!result.regime.id.empty());
    CHECK(result.lifecycle != EngineLifecycleState::COLD);
    CHECK(result.features.values.size() > 10u);
    CHECK(result.withinBudget);
  }
  {
    try {
      throw PipelineStageError(PipelineStage::FeatureGeneration, "boom");
    } catch (const PipelineStageError& e) {
      CHECK(e.stage == PipelineStage::FeatureGeneration);
      CHECK(std::string(e.what()).find("feature_generation") != std::string::npos);
    }
  }

  if (failures) { std::cerr << failures << " failure(s)\n"; return 1; }
  std::cout << "test_prediction_engine OK\n";
  return 0;
}
