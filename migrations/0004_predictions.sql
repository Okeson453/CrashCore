
CREATE TABLE IF NOT EXISTS predictions (
  id TEXT PRIMARY KEY,
  round_id TEXT NOT NULL,
  game_id TEXT,
  decision SMALLINT NOT NULL DEFAULT 0,
  confidence DOUBLE PRECISION DEFAULT 0,
  probability DOUBLE PRECISION DEFAULT 0,
  entry_mult DOUBLE PRECISION DEFAULT 0,
  target_mult DOUBLE PRECISION DEFAULT 0,
  correlation_id TEXT,
  model_version TEXT,
  regime_id TEXT,
  created_at_ms BIGINT,
  created_at TIMESTAMPTZ DEFAULT NOW()
);
CREATE INDEX IF NOT EXISTS idx_predictions_round ON predictions(round_id);
CREATE INDEX IF NOT EXISTS idx_predictions_created ON predictions(created_at_ms DESC);
CREATE INDEX IF NOT EXISTS idx_predictions_model ON predictions(model_version);
