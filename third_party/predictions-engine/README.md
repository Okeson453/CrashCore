# Predictions Engine — C++23 Port

Self-contained port of the TypeScript `predictions-engine` module. **No imports
from outside `pe/`** (no observability/, ensemble/, calibration/, Redis, etc.).

## Scope (in)

| TS | C++ |
|----|-----|
| `types.ts` | `include/pe/types.hpp` |
| `state/incremental-state-engine.ts` | `state/incremental_state_engine.{hpp,cpp}` |
| `features/calculators.ts` | `features/calculators.hpp` (full: mean/std/percentile/computeFeatures) |
| `features/feature-engine.ts` | `features/feature_engine.hpp` (V1) |
| `features/feature-engine-v2.ts` | `features/feature_engine_v2.hpp` |
| `features/*-features.ts` (9 families) | `features/{lag,run,markov,spectral,entropy,time,cross_target,gap}.hpp` |
| `features/feature-meta.ts` | `features/feature_meta.hpp` |
| `features/feature-version-assert.ts` | `features/feature_version_assert.hpp` |
| `features/incremental-features.ts` | `features/incremental_features.hpp` |
| `features/feature-importance.ts` | `features/feature_importance.hpp` |
| `regimes/*` | `regimes/{regime_state,regime_detector,learned_clustering,regime_fit_job}.hpp` |
| `models/baseline-model.ts` | `models/baseline_model.hpp` |
| `models/candidate-models.ts` + thin wrappers | `models/candidate_models.hpp` |
| `models/gap-conditional-model.ts` | `models/gap_conditional_model.hpp` (Model A/B) |
| `models/meta-logistic-model.ts` | `models/meta_logistic_model.hpp` |
| `models/feature-contribution.ts` | `models/feature_contribution.hpp` |
| `models/model-registry.ts` | `models/model_registry.hpp` |
| `signals/{signal,validate}` | `signals/{signal,validate}.hpp` |
| `prediction-engine.ts` | `engine/prediction_engine.hpp` |
| util (uuid/time/logger) | `util/{uuid,time,logger}.hpp` |

## Scope (out — by design)

`prediction-pipeline.ts`, `state-persistence.ts`, ensemble/calibration/
multi-target/opportunity/strategy/stake/lifecycle/drift/lookahead/validation,
`acie/`, `live/`, `backtesting/`, `workers/`.

## Deliberate deviations

1. **FEATURE_SCHEMA_V2** fixed to match actual family outputs.
2. **regime_adjusted boost** gated off by default (`allowBoost=false`).

## Build

```bash
# CMake
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release && cmake --build build -j
./build/pe_smoke && ./build/pe_demo

# Or direct
g++ -std=c++23 -O2 -Iinclude src/state/incremental_state_engine.cpp \
    examples/demo.cpp -o pe_demo
```

Requires C++23 (GCC 13+ / Clang 16+). Only dependency is the C++ standard library.


## Latency budget

| Metric | Budget |
|--------|--------|
| Soft target (typical predict) | **≤ 1 ms** |
| Hard ceiling (p99 / max) | **≤ 5 ms** |
| State `update()` alone | **≤ 0.1 ms** |

Measured on the V2 incremental critical path (no I/O, no full-history scans):

- `observe` ≈ **0.4 µs**
- `predict` p50 ≈ **13–20 µs**, p99 ≈ **35–80 µs** (well under 1 ms)

Constants live in `include/pe/util/latency_budget.hpp`. Each `PredictResult`
carries `latencyUs` and `withinBudget`. Spectral features are capped at 64
newest points (`SPECTRAL_CAP`) so cost stays O(1) vs lag-ring size.


## Optional ML path (`PE_ENABLE_ML`)

Enabled by default in CMake (`-DPE_ENABLE_ML=ON`). Adds:

- `include/pe/ml/*` — preprocessor, normalizer, inference, calibrator, fusion, runtime, loader, health, metrics
- `models/ml_predictive_model.hpp` — `PredictiveModel` adapter (candidate only, never default)
- Binary model format `PEML` v1 (logistic weights + optional norm + Platt/isotonic)
- Training scripts under `training/` (Python, separate from C++ build)

Hot path rules: no filesystem, network, JSON, locks, or Python during `predict`.
Model load happens once in `MLRuntime::loadFromConfig` before `LIVE`.

Measured ML path (preprocess → infer → calibrate): **~5 µs max** in tests (budget 1–5 ms).

Rollout: SHADOW (`ml_weight=0`) → CANARY (0.15–0.3) → PRODUCTION (trained fusion).
