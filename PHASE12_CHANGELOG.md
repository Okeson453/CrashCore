# Phase 1 + Phase 2 — Durable Delivery & ACIE Decision Surface

**Date:** 2026-09-18  
**Depends on:** Phase 0 (`PHASE0_CHANGELOG.md`)

## Phase 1 — Durable transactional path & temporal invariants

### New modules
| File | Role |
|------|------|
| `include/delivery/outbox/durable_prediction_tx.hpp` | Atomic-ish persist: predictions + target_claims + notification_outbox, then memory wake |
| `include/delivery/outbox/temporal_auth.hpp` | Pre-send authorization + BG target-start stale kill |

### Behaviors
1. **Response path** (`Application` prediction handler): after successful `registerPrediction` + claim, calls `DurablePredictionTx::persistPrediction`:
   - INSERT `predictions`
   - INSERT `target_claims` (round uniqueness)
   - INSERT `notification_outbox` with `round_id`
   - COMMIT
   - Memory outbox publish for immediate worker wake
   - Falls back to memory-only when DB is absent
2. **Outcome path**: persists into `outcomes` when DB present; always updates adaptive edge
3. **BG / Start / Prepare**: `TemporalGuard::noteTargetStarted` + `killStaleForTarget` dead-letters pending/claimed outbox rows for that round
4. **Pre-send**: `OutboxWorker` deliver lambda rejects sends when target already started or signal past delivery deadline
5. **OutboxRepository**: `enqueueWithRound` stores `round_id` for temporal kill

## Phase 2 — ACIE strategy surface & adaptive edge

### New modules
| File | Role |
|------|------|
| `include/prediction/acie/strategy.hpp` | ENTRY / REDUCED_ENTRY / SKIP evaluation vs fair odds + edge |
| `include/prediction/acie/adaptive_edge.hpp` | Online edge adjustment from WIN/LOSS outcomes |

### Behaviors
1. **`InProcessPredictionClient::submit`** uses `acie::evaluateStrategy` with:
   - fair = 1/target
   - quality edge (adaptive when available)
   - warm-sample floor
   - legacy fair-odds gate as safety net
2. **Strategy modes**: Quality, HighFrequency (reduced edge), Conservative (extra margin)
3. **AdaptiveEdge** wired into Application; outcomes call `noteOutcome`; client reads `currentEdge()` for gates

## Tests
- `tests/phase12_durable_acie_test.cpp` — strategy entry/skip/reduced/cold, adaptive edge motion, temporal deadline/start reject, durable memory fallback
- Hooked from `tests/test_main.cpp`
- Listed in `CMakeLists.txt`

## Still open (later phases)
- Full ACIE PSI ensemble / model registry / provenance / evidence
- Exactly-once feedback job table + idempotency keys (schema 0039+)
- Complete notification-worker claim/lease parity with TestingEngine priorities
- `wr_utils.wasm` signing path
- Frontend mock removal + auth
- Migrations 0039–0050 from TestingEngine
