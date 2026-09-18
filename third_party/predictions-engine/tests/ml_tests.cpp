#include "pe/engine/prediction_engine.hpp"
#include "pe/features/feature_engine_v2.hpp"
#include "pe/models/ml_predictive_model.hpp"
#include "pe/models/model_registry.hpp"
#include "pe/state/incremental_state_engine.hpp"
#include "pe/ml/ml_calibrator.hpp"

#include "pe/ml/ml_calibrator.hpp"
#include "pe/ml/ml_feature_vector.hpp"
#include "pe/ml/ml_fusion.hpp"
#include "pe/ml/ml_model_loader.hpp"
#include "pe/ml/ml_predictor.hpp"
#include "pe/ml/ml_preprocessor.hpp"
#include "pe/ml/ml_runtime.hpp"
#include "pe/util/latency_budget.hpp"

#include <cmath>
#include <iostream>
#include <string>

static int failures = 0;
#define CHECK(c) do { if (!(c)) { std::cerr << "FAIL " << #c << " @" << __LINE__ << "\n"; ++failures; } } while(0)
#define CHECK_NEAR(a,b,e) CHECK(std::abs((a)-(b))<=(e))

int main() {
  using namespace pe;
  using namespace pe::ml;

  // Preprocessor ordering + finite check
  {
    FeatureVector fv;
    fv.values["ewma_hit_13"] = 0.7;
    fv.values["markov_p_up"] = 0.6;
    std::array<double, MAX_ML_FEATURES> buf{};
    std::size_t n = 0;
    CHECK(preprocess(fv, buf.data(), buf.size(), &n));
    CHECK(n == mlFeatureOrder().size());
    // NaN rejection
    fv.values["ewma_hit_13"] = std::numeric_limits<double>::quiet_NaN();
    CHECK(!preprocess(fv, buf.data(), buf.size(), &n));
  }

  // Default model + inference
  {
    auto loaded = makeDefaultLogisticModel(mlFeatureOrder().size());
    CHECK(loaded.model != nullptr);
    CHECK(loaded.model->featureCount() == mlFeatureOrder().size());
    std::vector<double> x(loaded.model->featureCount(), 0.0);
    // set ewma index
    const auto& order = mlFeatureOrder();
    for (std::size_t i = 0; i < order.size(); ++i)
      if (order[i] == "ewma_hit_13") x[i] = 0.8;
    MLFeatureVector fv{x.data(), x.size()};
    auto r = loaded.model->predict(fv);
    CHECK(r.valid);
    CHECK(r.probability > 0.01 && r.probability < 0.99);
  }

  // Calibrator Platt near-identity
  {
    auto cal = MLCalibrator::platt(1.0, 0.0);
    CHECK_NEAR(cal.calibrate(0.65), 0.65, 0.02);
  }

  // Fusion fixed weight
  {
    auto f = MLFusion::fixedWeight(0.25);
    FusionInput in{0.70, 0.50, 0.8, 0.5, 0.0, 0.5};
    auto out = f.fuse(in);
    CHECK_NEAR(out.probability, 0.70 * 0.75 + 0.50 * 0.25, 0.02);
  }

  // Runtime LIVE + predictor
  {
    MLRuntime rt;
    CHECK(rt.loadDefault());
    CHECK(rt.state() == MLRuntimeState::Live);
    CHECK(rt.activeModel() != nullptr);

    IncrementalStateEngine eng;
    for (int i = 0; i < 80; ++i) eng.update(1.0 + (i % 4) * 0.4);
    FeatureEngineV2 fe;
    auto features = fe.compute(eng, "ml-test");

    MLPredictor pred(&rt);
    auto ml = pred.predict(features);
    CHECK(ml.has_value());
    CHECK(ml->valid);
    CHECK(ml->probability >= 0.01 && ml->probability <= 0.99);
  }

  // Latency: ML path under 5ms (and typically << 1ms)
  {
    MLRuntime rt;
    rt.loadDefault();
    IncrementalStateEngine eng;
    for (int i = 0; i < 100; ++i) eng.update(1.2 + (i % 3) * 0.3);
    FeatureEngineV2 fe;
    auto features = fe.compute(eng, "lat");
    MLPredictor pred(&rt);
    for (int i = 0; i < 30; ++i) pred.predict(features); // warmup
    double max_us = 0.0;
    for (int i = 0; i < 500; ++i) {
      auto t0 = std::chrono::steady_clock::now();
      auto ml = pred.predict(features);
      auto t1 = std::chrono::steady_clock::now();
      double us = std::chrono::duration<double, std::micro>(t1 - t0).count();
      if (us > max_us) max_us = us;
      CHECK(ml.has_value());
    }
    CHECK(max_us < static_cast<double>(LATENCY_BUDGET_US));
    std::cout << "ml_path_max_us: " << max_us << "\n";
  }

  // Registry integration
  {
    ModelRegistry reg;
    auto* m = reg.get("ml-predictive");
    CHECK(m != nullptr);
    CHECK(reg.getDefault().identity().name == "baseline-statistical");
  }

  // Full engine with ML model name
  {
    PredictionEngine engine;
#ifdef PE_ENABLE_ML
    if (auto* rt = engine.models().mlRuntime()) {
      // CANARY weight for test
      if (auto* mlp = dynamic_cast<MLPredictiveModel*>(engine.models().get("ml-predictive"))) {
        mlp->setMlWeight(0.2);
        mlp->setRuntime(rt);
      }
    }
#endif
    for (int i = 0; i < 60; ++i)
      engine.observe(1.1 + (i % 5) * 0.25, 1'700'000'000'000LL + i * 22'000);
    PredictRequest req;
    req.targetRoundId = "ml-e2e";
    req.target = 1.3;
    req.modelName = "ml-predictive";
    auto result = engine.predict(req);
    CHECK(result.signal.probability >= 0.01);
    CHECK(result.signal.probability <= 0.99);
    CHECK(result.withinBudget);
  }

  if (failures == 0) {
    std::cout << "All ML tests passed.\n";
    return 0;
  }
  std::cerr << failures << " failure(s)\n";
  return 1;
}
