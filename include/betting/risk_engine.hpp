#pragma once
#include <algorithm>
#include <cmath>
namespace crashcore {
struct RiskInput {
  double bankroll=1000;
  double probability=0.5;
  double odds=2.0; // decimal
  double maxFraction=0.05;
};
struct RiskDecision {
  double stake=0;
  double kellyFraction=0;
  bool allowed=false;
  const char* reason="ok";
};
class RiskEngine {
public:
  RiskDecision evaluate(const RiskInput& in) const {
    RiskDecision d;
    if (in.bankroll<=0 || in.probability<=0 || in.probability>=1) {
      d.reason="invalid_input"; return d;
    }
    // Kelly: f* = p - (1-p)/(b) where b=odds-1
    double b = std::max(0.01, in.odds-1.0);
    double f = in.probability - (1.0-in.probability)/b;
    f = std::clamp(f, 0.0, in.maxFraction);
    d.kellyFraction = f;
    d.stake = in.bankroll * f;
    d.allowed = f > 0.001;
    if (!d.allowed) d.reason="kelly_non_positive";
    return d;
  }
};
} // namespace crashcore
