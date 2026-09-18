#include "validation/validator.hpp"
#include "validation/prediction_registry.hpp"
#include "validation/outcome_matcher.hpp"
#include "timing/timestamp.hpp"
#include <cstdio>

using namespace crashcore;
static int failures = 0;
#define CHECK(c) do { if (!(c)) { std::printf("FAIL %s:%d %s\n", __FILE__, __LINE__, #c); ++failures; } } while(0)

int main() {
  // Win
  {
    Validator v;
    PredictionEvent pe;
    pe.predictionId = "w1";
    pe.targetRoundId = "r1";
    pe.decision = PredictionDecision::Enter;
    pe.targetMult = 2.0;
    pe.createdAtMs = nowMs();
    CHECK(v.onPrediction(pe));
    CrashEvent end;
    end.kind = EventKind::End;
    end.valid = true;
    end.roundId = "r1";
    end.gameId = "r1";
    end.crashPoint = 5.0;
    auto o = v.onRoundEnd(end);
    CHECK(o.size() == 1 && o[0].isWin);
  }
  // Duplicate prediction
  {
    PredictionRegistry reg;
    PredictionEvent pe;
    pe.predictionId = "d1";
    pe.targetRoundId = "r2";
    pe.createdAtMs = nowMs();
    CHECK(reg.registerPrediction(pe));
    auto r2 = reg.registerPrediction(pe);
    CHECK(!r2);
  }
  // Stale expiry
  {
    PredictionRegistry reg;
    PredictionEvent pe;
    pe.predictionId = "s1";
    pe.targetRoundId = "r3";
    pe.createdAtMs = nowMs() - 1'000'000;
    CHECK(reg.registerPrediction(pe));
    auto n = reg.expireStale(1000);
    CHECK(n == 1);
    CHECK(reg.pendingCount() == 0);
  }
  // Skip decision → void
  {
    Validator v;
    PredictionEvent pe;
    pe.predictionId = "v1";
    pe.targetRoundId = "r4";
    pe.decision = PredictionDecision::Skip;
    pe.createdAtMs = nowMs();
    CHECK(v.onPrediction(pe));
    CrashEvent end;
    end.kind = EventKind::End;
    end.valid = true;
    end.roundId = "r4";
    end.gameId = "r4";
    end.crashPoint = 10.0;
    auto o = v.onRoundEnd(end);
    CHECK(o.size() == 1 && o[0].result == PredictionOutcome::Void);
  }
  if (failures) { std::printf("%d failures\n", failures); return 1; }
  std::printf("validator_tests OK\n");
  return 0;
}
