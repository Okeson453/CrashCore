
CREATE TABLE IF NOT EXISTS walk_forward_runs (
  id TEXT PRIMARY KEY,
  started_at_ms BIGINT,
  finished_at_ms BIGINT,
  folds INT,
  metrics_json TEXT,
  status TEXT DEFAULT 'pending'
);
