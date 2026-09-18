# CrashCore

C++23 production runtime for BC.Game crash ingestion, prediction, validation,
durable outbox delivery, and observability — with a Next.js statistics frontend.

Repository: https://github.com/Okeson453/CrashCore

## Components

| Layer | Role |
|-------|------|
| **Ingestion** | Native / Socket.IO client, decoder, realtime pipeline |
| **Prediction** | In-process PE (`third_party/predictions-engine`) + ACIE strategy gate |
| **Validation** | Live validator, outcome matching, loss cooldown |
| **Delivery** | Durable prediction TX, temporal auth, notification worker, Telegram |
| **Persistence** | PostgreSQL (libpq), migrations, outbox, feedback |
| **Frontend** | Next.js App Router statistics observatory (`frontend/`) |

## Architecture

```
BC.Game → Socket → Decoder → RealtimePipeline → RoundState → EventRouter
                                      ↓
                         PredictionClient (InProcess PE)
                                      ↓
              register → claim → DurablePredictionTx → Outbox → Telegram
                                      ↓
                         Outcome → ExactlyOnceFeedback → AdaptiveEdge
```

## Decision contract

Predictions use a **fair-odds + quality edge** gate (default target **1.30×**), not a
hardcoded 0.5 probability. Strategy modes: `ENTRY` / `REDUCED_ENTRY` / `SKIP`.
Adaptive edge updates from realized win/loss outcomes (exactly-once feedback).

## Build (C++)

**Dependencies:** CMake ≥ 3.20, C++23 compiler, OpenSSL, Boost.System, libpq, libcurl, nlohmann_json.

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)
./build/crashcore_tests
./build/CrashCore
```

GitHub Actions CI builds **Debug** and **Release** and runs the test suite on every push to `main`.

## Frontend

```bash
cd frontend
npm install
npm run build
npm run dev
```

Environment (optional):

| Variable | Purpose |
|----------|---------|
| `CRASHCORE_API_URL` | C++ StatisticsServer origin (default `http://127.0.0.1:8080`) |
| `NEXT_PUBLIC_ALLOW_DEMO_FALLBACK` | Set `true` to allow mock data when backend is down (off by default) |
| `NEXT_PUBLIC_STATS_WS_URL` | Optional WebSocket URL for live stats |
| `NEXT_PUBLIC_FORCE_DEMO` | Force offline/demo mode |

Deploy the `frontend/` directory as the Vercel project root (or set Root Directory to `frontend`).

## Configuration (runtime)

Key `ApplicationConfig` flags:

- `enablePersistence` — require valid DB; connect + migrate hard-fail if misconfigured
- `enableTelegram` / `enableNotificationWorker` — delivery path
- `statsPort` — HTTP statistics server (default `8080`)
- `qualityEdge` / `minConfidence` / `deliveryDeadlineMs` — strategy & temporal policy

Database credentials via `DATABASE_URL` (see `include/security/credentials.hpp`).

## Migrations

SQL under `migrations/` (`0001` … `0050`). Applied by `MigrationRunner` when persistence is enabled.

## Layout

```
include/     Public headers (application, ingestion, prediction, delivery, …)
src/         Translation units linked into crashcore_lib / CrashCore
tests/       Unit, phase, and integration tests
frontend/    Next.js statistics UI
migrations/  PostgreSQL schema
third_party/ predictions-engine (in-process PE)
.github/     CI workflow
```

## License

See [LICENSE](LICENSE).
