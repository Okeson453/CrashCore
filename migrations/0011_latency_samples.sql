
CREATE TABLE IF NOT EXISTS latency_samples (
  id BIGSERIAL PRIMARY KEY,
  stage TEXT NOT NULL,
  us BIGINT NOT NULL,
  correlation_id TEXT,
  created_at_ms BIGINT NOT NULL
);
CREATE INDEX IF NOT EXISTS idx_latency_stage ON latency_samples(stage, created_at_ms DESC);
