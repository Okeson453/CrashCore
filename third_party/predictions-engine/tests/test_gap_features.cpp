#include "pe/features/feature_engine_v2.hpp"
#include "pe/features/feature_meta.hpp"
#include "pe/features/gap.hpp"
#include "pe/state/incremental_state_engine.hpp"
#include <cmath>
#include <iostream>
#include <string>

static int failures = 0;
#define CHECK(c) do { if (!(c)) { std::cerr << "FAIL " << #c << " @" << __LINE__ << "\n"; ++failures; } } while (0)
#define NEAR(a,b,e) CHECK(std::abs((a)-(b)) <= (e))

int main() {
  using namespace pe;

  {
    IncrementalStateEngine eng;
    eng.recordBeganAt(1'700'000'000'000LL);
    eng.update(1.5);
    eng.recordBeganAt(1'700'000'000'000LL + 25'000); // 25s
    eng.update(1.2);
    auto g = computeGapFeatures(eng);
    NEAR(g.at("gap_s"), 25.0, 1e-6);
    NEAR(g.at("gap_count"), 1.0, 1e-9);
    CHECK(g.count("log_lag_1") == 1);
  }
  {
    IncrementalStateEngine eng;
    for (int i = 0; i < 30; ++i) {
      eng.recordBeganAt(1'700'000'000'000LL + i * 20'000);
      eng.update(1.0 + (i % 5) * 0.3);
    }
    FeatureEngineV2 fe;
    auto fv = fe.compute(eng, "r1");
    CHECK(fv.values.count("gap_s") == 1);
    CHECK(fv.values.count("lag_1") == 1);
    CHECK(fv.values.count("markov_p_up") == 1);
    CHECK(fv.values.count("entropy_binary_13") == 1);
    CHECK(fv.featureVersion == std::string(FEATURE_VERSION_V2));
    CHECK(fv.meta.sampleSize == 30);
  }

  if (failures) { std::cerr << failures << " failure(s)\n"; return 1; }
  std::cout << "test_gap_features OK\n";
  return 0;
}
