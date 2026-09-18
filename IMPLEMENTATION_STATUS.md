# CrashCore Implementation Status (post-audit remediation)

## P0 Connectivity
- BeastWebSocketConnection is the default via `makeWebSocketConnection` in WebSocketClient
- Statistics HTTP server on :8080 (`StatisticsServer`) for frontend GET /api/statistics/*
- PgDatabase factory (`makeDatabase`) available; MigrationRunner loads 38 SQL files
- InProcessPredictionClient + N1Coordinator wired for End→N+1 PE path
- CurlHttpClient for production HTTP

## P1 Subsystems added
- Ensemble orchestrator, Platt + Isotonic calibrators, Drift detector
- Walk-forward engine, Dataset builder
- Conditional/Autocorr/Bayesian/Momentum/Volatility models
- Auth session store + password hash helpers
- Betting RiskEngine (Kelly)
- Funnel metrics, online stats, feature store shards
- Learning scheduler

## Database
- migrations/0001–0038 SQL files
- schema_migrations tracking

## Frontend
- Read-only observatory; proxies to CRASHCORE_API_URL (:8080)
- Falls back to demo data only when backend unreachable

## Remaining production work
- Live BC.Game handshake requires ExternalSigner (wr_utils) credentials
- Enable PE_ENABLE_ML with trained model artifacts under models/production/
- Full ACIE port remains large (TE 18 files) — PE baseline ensemble is the active path
- NotificationWorker full path needs TELEGRAM_* env + Pg outbox
