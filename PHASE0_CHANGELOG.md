# Phase 0 — Wiring, Decision Gate, Ownership, Build Surface

**Date:** 2026-09-18  
**Scope:** High-leverage Critical/High fixes from `CrashCore_TestingEngine_Audit.md` that can be applied without a full ACIE port.

## Changes

### 1. Decision contract (B-12 / Critical)
- Replaced hardcoded `probability >= 0.5` gate with TestingEngine-aligned quality gate:
  - fair probability = `1 / targetMult` (default 1.30 → ≈0.7692)
  - require `probability >= fair + qualityEdge` (default edge 0.02 → threshold ≈0.789)
  - still require `confidence >= minConfidence` (default 0.55)
- Added `Options::qualityEdge`.

### 2. Application::init ordering (B-01, B-02, B-09, B-10 / Critical–High)
- **Persistence first:** DB connect + migrations run *before* `LiveSupervisor` construction.
- Persistence failures when `enablePersistence=true` are hard errors (no silent degrade).
- Migration `applyAll()` result is checked.
- `ColdStartSeeder` is retained as a member and `seed()` is invoked (no discarded temporary).
- `LiveBoot` is constructed with real deps (supervisor, seeder, poll worker, prewarm) and `run()`.
- `NotificationWorker` is constructed when durable outbox repo + Telegram are available.
- New members: `cold_seeder_`, `outbox_repo_`.

### 3. Response ownership (B-03, B-04, B-05 / Critical–High)
- Application installs the single authoritative prediction response handler after `PipelineLoop` construction (overwrites the temporary PipelineLoop handler).
- Target ownership: **registerPrediction first**, then `claim` / `markPredicted` only on success (eliminates claim-before-register leak).

### 4. Duplicate frame path (B-06 / High)
- `onBinaryFrame` no longer calls `native_socket_->injectFrame` after `pipeline_->onFrame` (removes double decode/route risk).

### 5. Lifecycle start/stop (B-08, B-11 / High)
- `start()` acquires supervisor authority, starts renew loop, starts `NotificationWorker`.
- `stop()` performs bounded drain, stops notification worker, LiveBoot shutdown, supervisor release.

### 6. Health surface (B-21 / Medium)
- `health()` summary includes db, poll, outbox worker, notification worker, batch writer, stats server, supervisor authority, live boot, telegram.

### 7. Build / tests (B-25 / High)
- CMake test target now includes integration, validation, and `phase0_wiring_test.cpp` sources when present.
- New regression tests for the fair-odds decision gate.

## Explicitly NOT in Phase 0 (remain open)

- Full ACIE engine / PSI / strategy / online-state / provenance port (Critical)
- Transactional prediction + outbox persistence on response path (Critical)
- BG temporal stale-signal kill and pre-send temporal authorization (Critical)
- Exactly-once feedback into online weights / adaptive edge (Critical)
- `wr_utils.wasm` / complete native signing path (Critical)
- TestingEngine migrations 0039–0050 (Critical)
- Frontend auth + TanStack Start parity / mock removal (Critical/High)
- Full hot/cold lane PE offload (High)
- Complete delivery forensics wiring (High)

## Next phases

- **Phase 1:** Durable transactional prediction/outbox path + temporal delivery invariants.
- **Phase 2:** ACIE decision surface port (strategy + fair-odds already scaffolded).
- **Phase 3:** Feedback / online learning + frontend contract alignment.
- **Phase 4:** Remaining observability, concurrency, and production hardening.
