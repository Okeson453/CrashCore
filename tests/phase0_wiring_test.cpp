/**
 * Phase 0 regression tests — wiring, decision gate, ownership ordering.
 * Linked into crashcore_tests when present.
 */
#include "prediction_interface/in_process_prediction_client.hpp"
#include "prediction_interface/prediction_response.hpp"
#include "pe/engine/prediction_engine.hpp"

#include <cassert>
#include <cmath>
#include <iostream>
#include <string>

namespace {

using crashcore::PredictionDecision;
using crashcore::decisionFromSignal;

void test_decision_gate_fair_odds() {
  pe::PredictionSignal skipLow{};
  skipLow.probability = 0.40;
  skipLow.confidence = 0.90;
  // 0.40 << fair(0.769)+edge → Skip
  assert(decisionFromSignal(skipLow, 0.55, 1.30, 0.02) == PredictionDecision::Skip);

  pe::PredictionSignal borderline{};
  borderline.probability = 0.50;
  borderline.confidence = 0.90;
  // Old bug accepted >=0.5; new gate requires ~0.789 → Skip
  assert(decisionFromSignal(borderline, 0.55, 1.30, 0.02) == PredictionDecision::Skip);

  pe::PredictionSignal enterOk{};
  enterOk.probability = 0.82;
  enterOk.confidence = 0.60;
  assert(decisionFromSignal(enterOk, 0.55, 1.30, 0.02) == PredictionDecision::Enter);

  pe::PredictionSignal lowConf{};
  lowConf.probability = 0.90;
  lowConf.confidence = 0.40;
  assert(decisionFromSignal(lowConf, 0.55, 1.30, 0.02) == PredictionDecision::Skip);

  pe::PredictionSignal zero{};
  zero.probability = 0.0;
  zero.confidence = 0.0;
  assert(decisionFromSignal(zero, 0.55) == PredictionDecision::Skip);

  std::cout << "  [PASS] decision_gate_fair_odds\n";
}

void test_decision_gate_default_target() {
  pe::PredictionSignal s{};
  s.probability = 0.80;
  s.confidence = 0.70;
  // default target 1.30, edge 0.02
  assert(decisionFromSignal(s, 0.55) == PredictionDecision::Enter);
  std::cout << "  [PASS] decision_gate_default_target\n";
}

} // namespace

// Called from test_main if linked; also provides standalone main when built alone.
#ifndef CRASHCORE_PHASE0_NO_MAIN
int phase0_wiring_tests() {
  std::cout << "Phase 0 wiring tests\n";
  test_decision_gate_fair_odds();
  test_decision_gate_default_target();
  std::cout << "Phase 0: all passed\n";
  return 0;
}
#endif
