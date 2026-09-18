
CREATE TABLE IF NOT EXISTS statistics_snapshots (
  id BIGSERIAL PRIMARY KEY,
  sequence BIGINT NOT NULL,
  total_rounds BIGINT DEFAULT 0,
  total_predictions BIGINT DEFAULT 0,
  wins BIGINT DEFAULT 0,
  losses BIGINT DEFAULT 0,
  skipped BIGINT DEFAULT 0,
  win_rate DOUBLE PRECISION,
  avg_probability DOUBLE PRECISION,
  avg_confidence DOUBLE PRECISION,
  latency_p95_us BIGINT,
  payload JSONB,
  created_at_ms BIGINT NOT NULL
);
CREATE INDEX IF NOT EXISTS idx_stats_seq ON statistics_snapshots(sequence DESC);
