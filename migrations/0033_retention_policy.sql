
CREATE TABLE IF NOT EXISTS retention_policy (
  table_name TEXT PRIMARY KEY,
  retain_days INT NOT NULL,
  last_prune_at_ms BIGINT
);
INSERT INTO retention_policy(table_name, retain_days) VALUES
 ('latency_samples', 7),
 ('reconnect_events', 30),
 ('health_snapshots', 14)
ON CONFLICT DO NOTHING;
