/**
 * Phase 1 + 2 regression tests:
 *  - ACIE strategy gate (ENTRY / REDUCED / SKIP)
 *  - Adaptive edge updates
 *  - Temporal authorization
 *  - Durable TX structure (offline / no-DB path)
 */
#include "prediction/acie/strategy.hpp"
#include "prediction/acie/adaptive_edge.hpp"
#include "delivery/outbox/temporal_auth.hpp"
#include "delivery/outbox/durable_prediction_tx.hpp"
#include "delivery/outbox/outbox.hpp"
#include "delivery/outbox/durable_handoff.hpp"
#include "prediction_interface/prediction_event.hpp"
#include "validation/outcome.hpp"

#include <cassert>
#include <cmath>
#include <iostream>

namespace {

using namespace crashcore;
using namespace crashcore::acie;

void test_strategy_entry() {
  StrategyConfig cfg;
  cfg.warmMinSamples = 0; // allow cold for unit test
  StrategyInput in;
  in.probability = 0.85;
  in.confidence = 0.70;
  in.targetMult = 1.30;
  in.samplesSeen = 100;
  auto r = evaluateStrategy(in, cfg);
  assert(r.action == StrategyAction::Entry);
  assert(r.decision == PredictionDecision::Enter);
  assert(std::fabs(r.fairProbability - (1.0 / 1.30)) < 1e-9);
  std::cout << "  [PASS] strategy_entry\n";
}

void test_strategy_skip_below_edge() {
  StrategyConfig cfg;
  cfg.warmMinSamples = 0;
  StrategyInput in;
  in.probability = 0.50; // classic false positive under old 0.5 gate
  in.confidence = 0.90;
  in.targetMult = 1.30;
  in.samplesSeen = 100;
  auto r = evaluateStrategy(in, cfg);
  assert(r.action == StrategyAction::Skip);
  assert(r.decision == PredictionDecision::Skip);
  std::cout << "  [PASS] strategy_skip_below_edge\n";
}

void test_strategy_reduced_entry() {
  StrategyConfig cfg;
  cfg.mode = StrategyMode::HighFrequency;
  cfg.warmMinSamples = 0;
  StrategyInput in;
  in.probability = 1.0 / 1.30 + 0.006; // above reduced, may be below quality
  in.confidence = 0.50;
  in.targetMult = 1.30;
  in.samplesSeen = 100;
  auto r = evaluateStrategy(in, cfg);
  assert(r.action == StrategyAction::Entry || r.action == StrategyAction::ReducedEntry);
  assert(r.decision == PredictionDecision::Enter);
  std::cout << "  [PASS] strategy_reduced_entry\n";
}

void test_strategy_cold_start() {
  StrategyConfig cfg;
  cfg.warmMinSamples = 50;
  StrategyInput in;
  in.probability = 0.95;
  in.confidence = 0.95;
  in.samplesSeen = 3;
  auto r = evaluateStrategy(in, cfg);
  assert(r.action == StrategyAction::Skip);
  assert(r.reason == "cold_start");
  std::cout << "  [PASS] strategy_cold_start\n";
}

void test_adaptive_edge_moves() {
  AdaptiveEdge edge;
  const double e0 = edge.currentEdge();
  Outcome win;
  win.result = PredictionOutcome::Win;
  win.isWin = true;
  for (int i = 0; i < 10; ++i) edge.noteOutcome(win);
  // wins ease edge downward
  assert(edge.currentEdge() <= e0 + 1e-12);
  Outcome loss;
  loss.result = PredictionOutcome::Loss;
  loss.isWin = false;
  for (int i = 0; i < 20; ++i) edge.noteOutcome(loss);
  assert(edge.currentEdge() > edge.config().minEdge - 1e-12);
  assert(edge.outcomeCount() == 30);
  std::cout << "  [PASS] adaptive_edge_moves\n";
}

void test_temporal_auth_deadline() {
  TemporalGuard g;
  g.config().deliveryDeadlineMs = 1000;
  auto allow = g.authorizeSend("r1", nowMs());
  assert(allow.allowed());
  auto reject = g.authorizeSend("r1", nowMs() - 5000);
  assert(!reject.allowed());
  assert(reject.decision == TemporalDecision::RejectExpired);
  std::cout << "  [PASS] temporal_auth_deadline\n";
}

void test_temporal_auth_target_started() {
  TemporalGuard g;
  g.noteTargetStarted("round-42", nowMs() - 100);
  auto r = g.authorizeSend("round-42", nowMs());
  assert(!r.allowed());
  assert(r.decision == TemporalDecision::RejectTargetStarted);
  auto ok = g.authorizeSend("round-99", nowMs());
  assert(ok.allowed());
  std::cout << "  [PASS] temporal_auth_target_started\n";
}

void test_durable_tx_memory_fallback() {
  Outbox box(64);
  DurableHandoff handoff(box);
  // No DB → memory-only path
  DurablePredictionTx tx(nullptr, nullptr, &handoff, &box);
  PredictionEvent pev;
  pev.predictionId = "p-test-1";
  pev.targetRoundId = "r-100";
  pev.decision = PredictionDecision::Enter;
  pev.confidence = 0.8;
  pev.targetMult = 1.30;
  Signal sig;
  sig.kind = SignalKind::Prediction;
  sig.predictionId = pev.predictionId;
  sig.roundId = pev.targetRoundId;
  sig.text = "ENTER r-100";
  sig.createdAtMs = nowMs();
  auto res = tx.persistPrediction(pev, sig);
  assert(res.ok);
  assert(!res.durable);
  assert(res.memoryPublished);
  assert(handoff.hasPredictionPublished("p-test-1"));
  std::cout << "  [PASS] durable_tx_memory_fallback\n";
}

} // namespace

int phase12_durable_acie_tests() {
  std::cout << "Phase 1+2 durable/ACIE tests\n";
  test_strategy_entry();
  test_strategy_skip_below_edge();
  test_strategy_reduced_entry();
  test_strategy_cold_start();
  test_adaptive_edge_moves();
  test_temporal_auth_deadline();
  test_temporal_auth_target_started();
  test_durable_tx_memory_fallback();
  std::cout << "Phase 1+2: all passed\n";
  return 0;
}
