
CREATE TABLE IF NOT EXISTS outcomes (
  prediction_id TEXT PRIMARY KEY,
  round_id TEXT NOT NULL,
  result TEXT NOT NULL,
  actual_mult DOUBLE PRECISION,
  target_mult DOUBLE PRECISION,
  is_win BOOLEAN DEFAULT FALSE,
  resolved_at_ms BIGINT,
  created_at TIMESTAMPTZ DEFAULT NOW()
);
CREATE INDEX IF NOT EXISTS idx_outcomes_round ON outcomes(round_id);
CREATE INDEX IF NOT EXISTS idx_outcomes_result ON outcomes(result);
