
CREATE TABLE IF NOT EXISTS prediction_attempts (
  id TEXT PRIMARY KEY,
  round_id TEXT NOT NULL,
  started_at_ms BIGINT,
  completed_at_ms BIGINT,
  status TEXT,
  latency_us BIGINT,
  error TEXT
);
CREATE INDEX IF NOT EXISTS idx_pred_attempts_round ON prediction_attempts(round_id);
