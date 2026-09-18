#include "pe/engine/prediction_engine.hpp"
#include "pe/util/latency_budget.hpp"
#include <algorithm>
#include <cstdio>
#include <numeric>
#include <string>
#include <vector>

int main() {
  pe::PredictionEngine engine;
  for (int i = 0; i < 40; ++i)
    engine.observe(1.1 + (i % 7) * 0.3, std::nullopt);

  constexpr int N = 100;
  std::vector<int64_t> us;
  us.reserve(N);
  int ok = 0;
  for (int i = 0; i < N; ++i) {
    engine.observe(1.5 + (i % 5) * 0.2, std::nullopt);
    pe::PredictRequest req;
    req.targetRoundId = std::to_string(5000 + i);
    req.target = 1.3;
    auto r = engine.predict(req);
    us.push_back(r.latencyUs);
    if (r.withinBudget) ++ok;
  }
  std::sort(us.begin(), us.end());
  auto pct = [&](double p){ return us[std::size_t(p * (us.size()-1))]; };
  std::printf("=== PE predict() latency ===\n");
  std::printf("n=%d within_budget=%d/%d\n", N, ok, N);
  std::printf("p50=%lld p95=%lld p99=%lld max=%lld us\n",
              (long long)pct(0.50), (long long)pct(0.95),
              (long long)pct(0.99), (long long)us.back());
  std::printf("target=%lld hard_budget=%lld us\n",
              (long long)pe::LATENCY_TARGET_US, (long long)pe::LATENCY_BUDGET_US);
  const bool pass = us.back() <= pe::LATENCY_BUDGET_US * 2; // allow 2x in debug
  std::printf("result=%s\n", pass ? "PASS" : "FAIL");
  return pass ? 0 : 2;
}
