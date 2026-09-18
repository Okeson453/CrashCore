
CREATE TABLE IF NOT EXISTS health_snapshots (
  id BIGSERIAL PRIMARY KEY,
  healthy BOOLEAN,
  summary TEXT,
  payload JSONB,
  created_at_ms BIGINT NOT NULL
);
