
CREATE TABLE IF NOT EXISTS drift_events (
  id BIGSERIAL PRIMARY KEY,
  drift_type TEXT NOT NULL,
  feature_name TEXT,
  score DOUBLE PRECISION,
  threshold DOUBLE PRECISION,
  details TEXT,
  created_at_ms BIGINT NOT NULL
);
