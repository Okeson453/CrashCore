/**
 * Phase 3 + 4 tests: exactly-once feedback, forensics, temporal still holds.
 */
#include "feedback/exactly_once_feedback.hpp"
#include "feedback/delivery_forensics.hpp"
#include "prediction/acie/adaptive_edge.hpp"
#include "delivery/outbox/temporal_auth.hpp"
#include "validation/outcome.hpp"

#include <cassert>
#include <iostream>

namespace {

using namespace crashcore;

void test_exactly_once_feedback() {
  acie::AdaptiveEdge edge;
  ExactlyOnceFeedback fb(nullptr, &edge);
  Outcome o;
  o.predictionId = "pred-1";
  o.roundId = "r1";
  o.result = PredictionOutcome::Win;
  o.isWin = true;
  o.actualMult = 2.0;
  o.targetMult = 1.3;
  assert(fb.apply(o) == true);
  assert(fb.apply(o) == false); // duplicate
  assert(fb.appliedCount() == 1);
  assert(fb.duplicateCount() == 1);
  assert(edge.outcomeCount() == 1);
  std::cout << "  [PASS] exactly_once_feedback\n";
}

void test_feedback_ignores_non_terminal() {
  ExactlyOnceFeedback fb;
  Outcome o;
  o.predictionId = "pred-2";
  o.result = PredictionOutcome::Pending;
  assert(fb.apply(o) == false);
  assert(fb.appliedCount() == 0);
  std::cout << "  [PASS] feedback_ignores_non_terminal\n";
}

void test_delivery_forensics_trace() {
  DeliveryForensics f;
  f.note("c1", DeliveryStage::Published, "ob1");
  f.note("c1", DeliveryStage::Claimed);
  f.note("c1", DeliveryStage::Delivered);
  auto t = f.get("c1");
  assert(t.has_value());
  assert(t->stages.size() == 3);
  assert(f.eventCount() == 3);
  std::cout << "  [PASS] delivery_forensics_trace\n";
}

void test_temporal_still_rejects_started() {
  TemporalGuard g;
  g.noteTargetStarted("rx");
  auto r = g.authorizeSend("rx", nowMs());
  assert(!r.allowed());
  std::cout << "  [PASS] temporal_still_rejects_started\n";
}

} // namespace

int phase34_feedback_obs_tests() {
  std::cout << "Phase 3+4 feedback/obs tests\n";
  test_exactly_once_feedback();
  test_feedback_ignores_non_terminal();
  test_delivery_forensics_trace();
  test_temporal_still_rejects_started();
  std::cout << "Phase 3+4: all passed\n";
  return 0;
}
