/**
 * Self-contained smoke tests (no Catch2 dependency).
 * Covers state engine, features, baseline model, and full PredictionEngine path.
 */
#include "pe/engine/prediction_engine.hpp"
#include "pe/util/latency_budget.hpp"
#include "pe/features/feature_engine_v2.hpp"
#include "pe/models/baseline_model.hpp"
#include "pe/state/incremental_state_engine.hpp"

#include <cmath>
#include <iostream>
#include <string>

static int failures = 0;

#define CHECK(cond) do { \
  if (!(cond)) { \
    std::cerr << "FAIL: " << #cond << "  @ " << __FILE__ << ":" << __LINE__ << "\n"; \
    ++failures; \
  } \
} while (0)

#define CHECK_NEAR(a, b, eps) CHECK(std::abs((a) - (b)) <= (eps))

int main() {
  using namespace pe;

  // --- State engine ---
  {
    IncrementalStateEngine eng;
    CHECK(eng.count() == 0);
    CHECK(eng.getLifecycleState() == EngineLifecycleState::COLD);
    eng.update(1.1);
    eng.update(1.5);
    eng.update(2.5);
    CHECK(eng.count() == 3);
    CHECK(eng.hits().t13 == 2);
    CHECK(eng.hits().t20 == 1);
    CHECK(eng.roundsSince(1.3) == 0);
    CHECK(eng.roundsSince(5.0) == 3);

    // windowHitRate exact
    IncrementalStateEngine e2;
    for (int i = 0; i < 20; ++i) e2.update(1.0);
    for (int i = 0; i < 5; ++i) e2.update(2.0);
    CHECK_NEAR(e2.windowHitRate(5, 1.3), 1.0, 1e-9);
    CHECK_NEAR(e2.windowHitRate(25, 1.3), 5.0 / 25.0, 1e-9);

    // LAG_CAP
    IncrementalStateEngine e3;
    for (int i = 0; i < LAG_CAP + 50; ++i) e3.update(1.2);
    CHECK(e3.lagLen() == LAG_CAP);

    // lifecycle
    IncrementalStateEngine e4;
    for (int i = 0; i < 10; ++i) e4.update(1.4);
    CHECK(e4.getLifecycleState() == EngineLifecycleState::WARMING);
    for (int i = 0; i < 20; ++i) e4.update(1.4);
    CHECK(e4.getLifecycleState() == EngineLifecycleState::WARM);
    for (int i = 0; i < 80; ++i) e4.update(1.4);
    CHECK(e4.getLifecycleState() == EngineLifecycleState::PRODUCTION);

    // gap (30 seconds between starts)
    IncrementalStateEngine e5;
    e5.recordBeganAt(1'700'000'000'000LL);
    e5.recordBeganAt(1'700'000'000'000LL + 30'000);
    CHECK(e5.getGapState().gapCount == 1);
    CHECK_NEAR(e5.getGapState().lastGapS, 30.0, 1e-6);
  }

  // --- Features ---
  {
    IncrementalStateEngine eng;
    for (int i = 0; i < 40; ++i) {
      eng.recordBeganAt(1'000'000 + i * 20'000);
      eng.update(1.0 + (i % 5) * 0.3);
    }
    FeatureEngineV2 fe;
    auto fv = fe.compute(eng, "r1");
    CHECK(fv.values.count("gap_s") == 1);
    CHECK(fv.values.count("lag_1") == 1);
    CHECK(fv.values.count("markov_p_up") == 1);
    CHECK(fv.values.count("entropy_binary_13") == 1);
    CHECK(fv.featureVersion == std::string(FEATURE_VERSION_V2));
    CHECK(fv.meta.sampleSize == 40);
  }

  // --- Baseline model ---
  {
    IncrementalStateEngine eng;
    for (int i = 0; i < 80; ++i) eng.update(i % 3 == 0 ? 1.0 : 1.8);
    FeatureEngineV2 fe;
    auto fv = fe.compute(eng, "round-1");
    BaselineStatisticalModel model;
    auto out = model.predict(fv, 1.3, nullptr);
    CHECK(out.probability >= 0.01);
    CHECK(out.probability <= 0.99);
    CHECK(out.confidence >= 0.15);
    CHECK(out.confidence <= 0.92);
    CHECK(out.model.name == "baseline-statistical");
    CHECK(!out.predictionId.empty());
  }

  // --- Full engine ---
  {
    PredictionEngine engine;
    for (int i = 0; i < 60; ++i)
      engine.observe(1.0 + (i % 4) * 0.4, 1'700'000'000'000LL + i * 25'000);

    PredictRequest req;
    req.targetRoundId = "round-test-1";
    req.target = 1.3;
    auto result = engine.predict(req);
    CHECK(result.signal.targetRoundId == "round-test-1");
    CHECK(result.signal.probability >= 0.01);
    CHECK(result.signal.probability <= 0.99);
    CHECK(result.signal.featurePath == FeaturePath::V2_INCREMENTAL);
    CHECK(!result.regime.id.empty());
    CHECK(result.lifecycle != EngineLifecycleState::COLD);
    CHECK(result.features.values.size() > 10u);
  }

  // --- Latency budget (1 ms target / 5 ms hard ceiling) ---
  {
    PredictionEngine engine;
    for (int i = 0; i < 100; ++i)
      engine.observe(1.0 + (i % 5) * 0.3, 1'700'000'000'000LL + i * 20'000);
    PredictRequest req;
    req.targetRoundId = "latency";
    req.target = 1.3;
    // warmup
    for (int i = 0; i < 20; ++i) engine.predict(req);
    int64_t maxUs = 0;
    for (int i = 0; i < 200; ++i) {
      engine.observe(1.2 + (i % 3) * 0.2);
      auto r = engine.predict(req);
      if (r.latencyUs > maxUs) maxUs = r.latencyUs;
      CHECK(r.withinBudget);
    }
    CHECK(maxUs <= LATENCY_BUDGET_US);
    // Soft target: p-ish — allow occasional spikes but mean should be << 1ms
    CHECK(maxUs < LATENCY_BUDGET_US);
  }

  if (failures == 0) {
    std::cout << "All smoke tests passed.\n";
    return 0;
  }
  std::cerr << failures << " check(s) failed.\n";
  return 1;
}
