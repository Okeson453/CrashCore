-- Signal timeline for delivery forensics reconstruction
CREATE TABLE IF NOT EXISTS signal_timeline (
  id BIGSERIAL PRIMARY KEY,
  correlation_id TEXT NOT NULL,
  prediction_id TEXT,
  stage TEXT NOT NULL,
  detail TEXT,
  created_at_ms BIGINT NOT NULL
);
CREATE INDEX IF NOT EXISTS idx_signal_timeline_corr ON signal_timeline (correlation_id, created_at_ms);
