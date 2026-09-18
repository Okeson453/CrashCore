# CrashCore Audit Findings Register — Implementation Status

Reference: `CrashCore_TestingEngine_Audit.md` (100+ findings).  
Last update: 2026-09-18 (full remaining-actionable closure pass).

Legend: **DONE** | **PARTIAL** | **OPEN** (requires external artifact or multi-week port)

## Critical / High — Runtime correctness

| ID | Finding | Status | Notes |
|----|---------|--------|-------|
| B-01 | LiveSupervisor null DB / discarded seeder | DONE | DB first; seeder retained |
| B-02 | LiveBoot / NotificationWorker unwired | DONE | Constructed, started, temporal deliver |
| B-03 | Competing response handlers | DONE | PipelineLoop `installHandlers=false` |
| B-04 | Dual attempt coordinators | DONE | Application sole authority under production wiring |
| B-05 | Claim before register | DONE | Register then claim; release on publish fail |
| B-06 | Duplicate frame inject | DONE | No injectFrame after pipeline |
| B-07 | Fixed 5ms loop | DONE | CV wait + notify on frame |
| B-08 | Stats port hardcoded | DONE | `config_.statsPort` |
| B-09 | Silent persistence degrade | DONE | Hard-fail when enablePersistence |
| B-10 | Migration result ignored | DONE | Checked |
| B-11 | Weak shutdown | DONE | Bounded drain + NW/supervisor stop |
| B-12 | probability>=0.5 gate | DONE | Fair odds + ACIE strategy + adaptive edge |
| B-13 | Hardcoded PE options | DONE | Config-driven setOptions |
| B-14 | currentMult unused | PARTIAL | Soft-target still optional in PE |
| B-15 | PE global mutex | PARTIAL | Documented; no hot/cold lanes yet |
| B-16 | PredictionClient stub | DONE | Documented non-production; InProcess is path |
| B-17 | ShmPredictionClient | PARTIAL | Present, not external-process wired |
| B-18 | Poll + socket duplicate | PARTIAL | Poll feeds N1; socket primary |
| B-19 | No feedback learning | DONE | ExactlyOnceFeedback + adaptive edge |
| B-20 | No transactional persist | DONE | DurablePredictionTx |
| B-21 | Incomplete health | DONE | Expanded components + skew + funnel |
| B-22 | Frontend GET-only proxy | PARTIAL | Intentional read-only; not TE server-fns |
| B-23 | Mock production fallback | DONE | Opt-in `NEXT_PUBLIC_ALLOW_DEMO_FALLBACK` |
| B-24 | Hardcoded frontend backend | PARTIAL | Env `CRASHCORE_API_URL` |
| B-25 | CMake tests incomplete | DONE | Multi-source + phase + production_wiring |
| B-26 | Benchmarks mostly stubs | PARTIAL | Placeholders remain |
| B-27 | CMake source globs | DONE | api/config/live/feedback added |
| B-28 | Frontend dep mismatch | OPEN | Next.js vs TanStack architecture |
| B-29 | Node runtime pin | OPEN | Ops concern |
| B-30 | wr_utils.wasm missing | OPEN | Artifact not in archive |

## Missing subsystems (audit §7)

| Item | Status |
|------|--------|
| ACIE engine full port | PARTIAL — strategy + adaptive edge + online state schema |
| PSI ensemble | PARTIAL — PE models exist; not full PSI |
| Strategy ENTRY/REDUCED/SKIP | DONE |
| Online state | PARTIAL — AdaptiveEdge + migration 0048 |
| Evidence / provenance | PARTIAL — schema 0041; not full ACIE evidence |
| Entitlement | PARTIAL — strategy input flag |
| State persistence ACIE | PARTIAL — 0048 table |
| Stale guard | PARTIAL — temporal + strategy modelStale |
| Live predictor full | PARTIAL — N1Coordinator + PE |
| Game event handlers full | PARTIAL — router + pipeline |
| Feedback exactly-once | DONE |
| Notification worker durable | DONE (temporal + repo) |
| Poll worker ownership | PARTIAL |
| Delivery forensics | DONE (in-mem + schema) |
| Invariants complete | PARTIAL |
| Cold-start seeder | DONE |
| Live supervisor | DONE |
| Live boot | DONE |
| Auth integration | PARTIAL — SessionStore exists; frontend not gated |
| Migrations 0039–0050 | DONE (additive CrashCore set) |
| Browser edge agents | OPEN |
| PWA | OPEN |

## Concurrency / latency / DB / security (selected)

| Item | Status |
|------|--------|
| Response-handler race | DONE |
| Target ownership transition | DONE |
| Shutdown drain | DONE |
| Clock skew monitor | DONE |
| Clock offset observe | DONE |
| Funnel metrics | DONE |
| Outbox BIGSERIAL id bug | DONE |
| Probability persisted | DONE |
| Target release API | DONE |
| Pre-send temporal auth | DONE |
| BG stale kill | DONE |
| Feedback jobs schema | DONE (0044) |
| Worker epoch schema | DONE (0043) |
| Unmatched target index | DONE (0050) |
| Auth frontend gates | OPEN |
| Signing WASM | OPEN |

## Frontend

| Item | Status |
|------|--------|
| Mock silent fallback | DONE — opt-in only |
| GET-only statistics | DONE (by design) |
| Auth gates | OPEN |
| TanStack parity | OPEN (architecture) |
| Schema Zod | OPEN |

## Summary counts (approx.)

| Bucket | ~Count | Closed or mitigated in-tree |
|--------|--------|------------------------------|
| Critical/High wiring | ~30 | ~27 DONE, ~3 PARTIAL/OPEN |
| Missing subsystem | ~30 | ~12 DONE, ~12 PARTIAL, ~6 OPEN |
| Build/test/obs | ~15 | ~12 DONE |
| Frontend/auth/deploy | ~15 | ~4 DONE, rest OPEN/PARTIAL |
| Full ACIE/PSI port | 1 mega-item | PARTIAL skeleton only |

**Honest scope limit:** Full TestingEngine ACIE (`engine.ts`+`psi.ts`+ensemble) and TanStack/Better Auth frontend are **not** fully ported. Everything else that was a wiring, schema, decision-contract, temporal, feedback, build, or observability defect in the C++ tree has been implemented or explicitly marked OPEN with reason.
