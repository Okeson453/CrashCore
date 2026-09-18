/**
 * Full re-verification tests for Phases 0–4 gap closures.
 */
#include "prediction/acie/strategy.hpp"
#include "prediction/acie/adaptive_edge.hpp"
#include "delivery/outbox/temporal_auth.hpp"
#include "delivery/outbox/durable_prediction_tx.hpp"
#include "delivery/outbox/outbox.hpp"
#include "delivery/outbox/durable_handoff.hpp"
#include "feedback/exactly_once_feedback.hpp"
#include "prediction_interface/target_coordinator.hpp"
#include "prediction_interface/prediction_event.hpp"
#include "prediction_interface/in_process_prediction_client.hpp"
#include "common/types.hpp"

#include <cassert>
#include <iostream>

namespace {

using namespace crashcore;
using namespace crashcore::acie;

void test_probability_on_event() {
  PredictionResponse resp;
  resp.predictionId = "p1";
  resp.targetRoundId = "r1";
  resp.confidence = 0.9;
  resp.probability = 0.85;
  resp.decision = PredictionDecision::Enter;
  resp.valid = true;
  auto pev = fromResponse(resp, "g1");
  assert(pev.probability == 0.85);
  assert(pev.confidence == 0.9);
  std::cout << "  [PASS] probability_on_event\n";
}

void test_target_release() {
  TargetCoordinator tc;
  tc.noteRoundStart("r-rel");
  assert(tc.claim("r-rel", "pred-x"));
  assert(tc.release("r-rel"));
  // can claim again after release
  assert(tc.claim("r-rel", "pred-y"));
  std::cout << "  [PASS] target_release\n";
}

void test_strategy_rejects_legacy_half() {
  StrategyConfig cfg;
  cfg.warmMinSamples = 0;
  StrategyInput in;
  in.probability = 0.5;
  in.confidence = 0.99;
  in.targetMult = 1.30;
  in.samplesSeen = 100;
  auto r = evaluateStrategy(in, cfg);
  assert(r.decision == PredictionDecision::Skip);
  std::cout << "  [PASS] strategy_rejects_legacy_half\n";
}

void test_exactly_once_and_edge() {
  AdaptiveEdge edge;
  ExactlyOnceFeedback fb(nullptr, &edge);
  Outcome o;
  o.predictionId = "px";
  o.result = PredictionOutcome::Loss;
  o.isWin = false;
  assert(fb.apply(o));
  assert(!fb.apply(o));
  assert(edge.lossCount() == 1);
  std::cout << "  [PASS] exactly_once_and_edge\n";
}

void test_temporal_and_memory_durable() {
  TemporalGuard g;
  g.noteTargetStarted("rt");
  assert(!g.authorizeSend("rt", nowMs()).allowed());
  Outbox box(32);
  DurableHandoff h(box);
  DurablePredictionTx tx(nullptr, nullptr, &h, &box);
  PredictionEvent pev;
  pev.predictionId = "pd";
  pev.targetRoundId = "rt2";
  pev.probability = 0.9;
  pev.decision = PredictionDecision::Enter;
  Signal s;
  s.predictionId = "pd";
  s.roundId = "rt2";
  s.text = "ENTER";
  s.createdAtMs = nowMs();
  auto res = tx.persistPrediction(pev, s);
  assert(res.ok && res.memoryPublished && !res.durable);
  std::cout << "  [PASS] temporal_and_memory_durable\n";
}

void test_decision_from_signal_fair() {
  pe::PredictionSignal sig;
  sig.probability = 0.5;
  sig.confidence = 0.9;
  assert(decisionFromSignal(sig, 0.55, 1.30, 0.02) == PredictionDecision::Skip);
  sig.probability = 0.82;
  assert(decisionFromSignal(sig, 0.55, 1.30, 0.02) == PredictionDecision::Enter);
  std::cout << "  [PASS] decision_from_signal_fair\n";
}

} // namespace

int phase_reverify_tests() {
  std::cout << "Phase re-verify tests (P0–P4 gaps)\n";
  test_probability_on_event();
  test_target_release();
  test_strategy_rejects_legacy_half();
  test_exactly_once_and_edge();
  test_temporal_and_memory_durable();
  test_decision_from_signal_fair();
  std::cout << "Phase re-verify: all passed\n";
  return 0;
}
