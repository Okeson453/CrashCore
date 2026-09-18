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
    CHECK(eng.count() == 0);
    CHECK(eng.getLifecycleState() == EngineLifecycleState::COLD);
    CHECK(eng.hitRate(1.3) == 0.0);
  }
  {
    IncrementalStateEngine eng;
    eng.update(1.1);
    eng.update(1.5);
    eng.update(2.5);
    CHECK(eng.count() == 3);
    CHECK(eng.hits().t13 == 2);
    CHECK(eng.hits().t20 == 1);
    CHECK(eng.runs().below13 == 0);
    CHECK(eng.runs().above13 == 2);
    CHECK(eng.roundsSince(1.3) == 0);
    CHECK(eng.roundsSince(5.0) == 3);
  }
  {
    IncrementalStateEngine eng;
    for (int i = 0; i < 20; ++i) eng.update(1.0);
    for (int i = 0; i < 5; ++i) eng.update(2.0);
    NEAR(eng.windowHitRate(5, 1.3), 1.0, 1e-9);
    NEAR(eng.windowHitRate(25, 1.3), 5.0 / 25.0, 1e-9);
    NEAR(eng.windowHitRate(100, 1.3), 5.0 / 25.0, 1e-9);
  }
  {
    IncrementalStateEngine eng;
    for (int i = 0; i < 100; ++i) eng.update(i % 2 == 0 ? 1.0 : 2.0);
    auto w = eng.hitRateWindows13();
    CHECK(w.w20 >= 0.0 && w.w20 <= 1.0);
    CHECK(w.w50 >= 0.0 && w.w100 >= 0.0);
  }
  {
    IncrementalStateEngine eng;
    for (int i = 0; i < 10; ++i) eng.update(1.4);
    CHECK(eng.getLifecycleState() == EngineLifecycleState::WARMING);
    for (int i = 0; i < 20; ++i) eng.update(1.4);
    CHECK(eng.getLifecycleState() == EngineLifecycleState::WARM);
    for (int i = 0; i < 80; ++i) eng.update(1.4);
    CHECK(eng.getLifecycleState() == EngineLifecycleState::PRODUCTION);
  }
  {
    IncrementalStateEngine eng;
    for (int i = 0; i < LAG_CAP + 50; ++i) eng.update(1.0 + (i % 10) * 0.1);
    CHECK(eng.lagLen() == LAG_CAP);
    CHECK(eng.getLagArray().size() == static_cast<size_t>(LAG_CAP));
  }
  {
    IncrementalStateEngine eng;
    eng.seed({1.0, 2.0, 3.0, 1.2});
    CHECK(eng.count() == 4);
    eng.reset();
    CHECK(eng.count() == 0);
    CHECK(eng.getLifecycleState() == EngineLifecycleState::COLD);
  }
  {
    IncrementalStateEngine eng;
    eng.update(1.0);
    eng.update(2.0);
    eng.update(2.0);
    eng.update(1.0);
    CHECK(eng.markov().trans[0][1] >= 1);
    CHECK(eng.markov().trans[1][1] >= 1);
    CHECK(eng.markov().trans[1][0] >= 1);
  }
  {
    IncrementalStateEngine eng;
    eng.recordBeganAt(1'700'000'000'000LL);
    eng.recordBeganAt(1'700'000'000'000LL + 30'000);
    auto g = eng.getGapState();
    CHECK(g.gapCount == 1);
    NEAR(g.lastGapS, 30.0, 1e-6);
  }

  if (failures) { std::cerr << failures << " failure(s)\n"; return 1; }
  std::cout << "test_incremental_state_engine OK\n";
  return 0;
}
