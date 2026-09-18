#include "pe/features/feature_engine_v2.hpp"
#include "pe/models/baseline_model.hpp"
#include "pe/state/incremental_state_engine.hpp"
#include <cmath>
#include <iostream>

static int failures = 0;
#define CHECK(c) do { if (!(c)) { std::cerr << "FAIL " << #c << " @" << __LINE__ << "\n"; ++failures; } } while (0)
#define NEAR(a,b,e) CHECK(std::abs((a)-(b)) <= (e))

int main() {
  using namespace pe;

  {
    IncrementalStateEngine eng;
    for (int i = 0; i < 80; ++i) eng.update(i % 3 == 0 ? 1.0 : 1.8);
    FeatureEngineV2 fe;
    auto fv = fe.compute(eng, "round-1");
    BaselineStatisticalModel model;
    auto out = model.predict(fv, 1.3, nullptr);
    CHECK(out.probability >= 0.01 && out.probability <= 0.99);
    CHECK(out.confidence >= 0.15 && out.confidence <= 0.92);
    CHECK(out.model.name == "baseline-statistical");
    CHECK(!out.predictionId.empty());
  }
  {
    IncrementalStateEngine eng;
    eng.update(1.0);
    eng.update(1.0);
    FeatureEngineV2 fe;
    auto fv = fe.compute(eng, "cold");
    BaselineStatisticalModel model;
    auto out = model.predict(fv, 1.3, nullptr);
    // Cold sample: window rates ~0, soft-shrink pulls toward EMPIRICAL_BASE_1_30
    CHECK(out.probability > 0.01 && out.probability < EMPIRICAL_BASE_1_30);
    CHECK(out.probability > 0.3); // evidence of shrink toward ~0.77
  }
  {
    BaselineStatisticalModel model;
    model.observeOutcome(0.8, 0);
    model.observeOutcome(0.7, 1);
    CHECK(model.adaptiveState().outcomes.size() == 2);
    CHECK(model.adaptiveState().rollingAbsError > 0.0);
  }

  if (failures) { std::cerr << failures << " failure(s)\n"; return 1; }
  std::cout << "test_baseline_model OK\n";
  return 0;
}
