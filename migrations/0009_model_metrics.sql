
CREATE TABLE IF NOT EXISTS model_metrics (
  id BIGSERIAL PRIMARY KEY,
  model_name TEXT NOT NULL,
  model_version TEXT,
  predictions BIGINT DEFAULT 0,
  wins BIGINT DEFAULT 0,
  losses BIGINT DEFAULT 0,
  win_rate DOUBLE PRECISION,
  avg_probability DOUBLE PRECISION,
  avg_confidence DOUBLE PRECISION,
  calibration_error DOUBLE PRECISION,
  window_start_ms BIGINT,
  window_end_ms BIGINT,
  created_at_ms BIGINT NOT NULL
);
CREATE INDEX IF NOT EXISTS idx_model_metrics_name ON model_metrics(model_name, created_at_ms DESC);
