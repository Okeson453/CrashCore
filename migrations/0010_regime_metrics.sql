
CREATE TABLE IF NOT EXISTS regime_metrics (
  id BIGSERIAL PRIMARY KEY,
  regime_id TEXT NOT NULL,
  samples BIGINT DEFAULT 0,
  predictions BIGINT DEFAULT 0,
  wins BIGINT DEFAULT 0,
  losses BIGINT DEFAULT 0,
  win_rate DOUBLE PRECISION,
  avg_probability DOUBLE PRECISION,
  created_at_ms BIGINT NOT NULL
);
CREATE INDEX IF NOT EXISTS idx_regime_metrics ON regime_metrics(regime_id, created_at_ms DESC);
