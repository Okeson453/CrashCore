
CREATE TABLE IF NOT EXISTS calibration_state (
  id TEXT PRIMARY KEY,
  model_name TEXT NOT NULL,
  method TEXT NOT NULL,
  params_json TEXT,
  sample_count BIGINT DEFAULT 0,
  ece DOUBLE PRECISION,
  updated_at_ms BIGINT NOT NULL
);
