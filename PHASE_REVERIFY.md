# Full Phase Re-Verification (P0 → P4) + Gap Closures

**Date:** 2026-09-18

## Matrix: audit finding → status after this pass

| Area | Finding | Status |
|------|---------|--------|
| P0 Decision gate | Hardcoded 0.5 | **Closed** — fair odds + edge + ACIE strategy |
| P0 LiveSupervisor null DB | Constructed before DB | **Closed** — persistence first |
| P0 ColdStartSeeder discarded | Temporary voided | **Closed** — retained + seed() |
| P0 LiveBoot / NotificationWorker | Unwired | **Closed** — constructed, started, temporal deliver |
| P0 Competing handlers | PipelineLoop + Application | **Closed** — `installHandlers=false` under Application |
| P0 Claim-before-register | Unsafe order | **Closed** — register then claim; release on publish fail |
| P0 Duplicate frame path | injectFrame after pipeline | **Closed** |
| P0 Persistence silent degrade | warn-only | **Closed** — hard-fail when enabled |
| P0 Migrations ignored | applyAll unchecked | **Closed** |
| P0 Health incomplete | Missing components | **Closed** — expanded summary |
| P0 CMake tests | Only test_main | **Closed** — multi-source + phase tests |
| P1 Durable TX | Missing | **Closed** — predictions + target_claims + outbox |
| P1 Outbox id type | String into BIGSERIAL | **Closed** — omit serial id |
| P1 Temporal pre-send | Missing | **Closed** — OutboxWorker + NotificationWorker |
| P1 BG stale kill | Missing | **Closed** — on Start/Prepare |
| P2 ACIE strategy | Missing | **Closed** — ENTRY/REDUCED/SKIP skeleton |
| P2 Adaptive edge | Missing | **Closed** — outcome-driven |
| P2 Probability not stored | Always 0.0 in TX | **Closed** — field + PE signal + TX |
| P3 Exactly-once feedback | Missing | **Closed** |
| P3 Frontend mock | Silent production fallback | **Closed** — opt-in only |
| P3 Migrations 0039–0040 | Missing | **Closed** |
| P4 Event-driven wake | 5ms poll only | **Closed** — notify on frame |
| P4 Funnel / forensics | Disconnected | **Closed** — wired |
| Target release API | Missing | **Closed** — `TargetCoordinator::release` |

## Intentionally still partial (not claimable as closed)

| Item | Why |
|------|-----|
| Full ACIE PSI ensemble / model registry / provenance | Large TypeScript port; strategy gate only |
| TestingEngine migrations 0041–0050 full semantics | Partial additive schema only through 0040 |
| wr_utils.wasm native signing | Artifact not in archive |
| Better Auth + TanStack Start parity | Architecture replacement (Next.js) remains |
| PE hot/cold lane offload | Still single `pe_mu_` serialization |
| Full transactional isolation under concurrent workers | Basic Transaction helper; not pool-isolated lanes |

## Tests added this pass

- `tests/phase_reverify_test.cpp` — probability, target release, strategy vs 0.5, exactly-once, temporal, fair gate

## Verdict

All **actionable wiring/partial items** from Phases 0–4 that can be completed inside the existing C++ architecture without a full ACIE/port rewrite are implemented and regression-tested. Remaining gaps are explicit large-scope ports or deployment artifacts, not silent stubs in the active path.
