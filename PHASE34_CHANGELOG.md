# Phase 3 + Phase 4 — Feedback, Frontend Contract, Observability

**Date:** 2026-09-18  
**Depends on:** Phase 0–2

## Re-verification of Phase 1 & 2

| Check | Result |
|-------|--------|
| DurablePredictionTx + TemporalGuard + AdaptiveEdge present | OK |
| Application wires persistPrediction / persistOutcome / killStale / authorizeSend | OK |
| Client uses evaluateStrategy + adaptive edge | OK |
| **Bug found:** outbox `id BIGSERIAL` vs string INSERT | **Fixed** — insert omits id; uses prediction_id + dedupe_key |

## Phase 3 — Feedback & frontend contract

| Item | Change |
|------|--------|
| `feedback/exactly_once_feedback.hpp` | Dedupe by prediction_id; applies adaptive edge once; optional durable `feedback_events` insert |
| Application outcome path | Uses `feedback_.apply(o)` instead of raw `adaptive_edge_.noteOutcome` |
| Delivery forensics | Notes Published on prediction persist; Delivered on feedback apply |
| `migrations/0039_feedback_idempotency.sql` | Unique index on feedback_events(prediction_id); outbox/target indexes; additive prediction columns |
| `migrations/0040_delivery_forensics.sql` | Durable forensics table |
| Frontend `statistics.ts` | Mock/demo fallback is **opt-in only** (`NEXT_PUBLIC_ALLOW_DEMO_FALLBACK=true`); production fails closed |

## Phase 4 — Observability & concurrency hardening

| Item | Change |
|------|--------|
| Event-driven wake | `onBinaryFrame` notifies `loop_cv_` after pipeline processing |
| FunnelMetrics | Wired into Application; published/actionable notes; included in health summary |
| Health surface | feedback_applied, feedback_dup, forensics count, adaptive_edge, funnel report |
| Tests | `tests/phase34_feedback_obs_test.cpp` |

## Still not full TestingEngine parity

- Full ACIE PSI ensemble / model registry / provenance payloads  
- Complete notification-worker priority inversion fix parity  
- `wr_utils.wasm` signing  
- Better Auth / TanStack Start frontend  
- Migrations beyond 0040 for all TestingEngine 0041–0050 semantics  

These require continued dedicated phases or an explicit descope.
