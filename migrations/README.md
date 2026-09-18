# CrashCore SQL Migrations

38 migration files covering auth, rounds, predictions, outcomes, outbox,
statistics, model/regime metrics, latency, decision audit, feedback,
calibration, drift, features, ensemble weights, walk-forward, datasets,
API keys, rate limits, telegram log, clock skew, reconnect, health,
prediction attempts, target claims, loss cooldown, forensics, equity,
system config, pipeline checkpoints, WS/poll logs.

Applied by `MigrationRunner` from the `migrations/` directory with
`schema_migrations` tracking.
