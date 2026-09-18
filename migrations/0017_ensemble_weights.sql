
CREATE TABLE IF NOT EXISTS ensemble_weights (
  id BIGSERIAL PRIMARY KEY,
  model_name TEXT NOT NULL,
  weight DOUBLE PRECISION NOT NULL,
  window_id TEXT,
  updated_at_ms BIGINT NOT NULL
);
