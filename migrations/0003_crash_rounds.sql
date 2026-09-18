
CREATE TABLE IF NOT EXISTS crash_rounds (
  game_id TEXT PRIMARY KEY,
  multiplier DOUBLE PRECISION NOT NULL,
  hash TEXT,
  salt TEXT,
  began_at_ms BIGINT,
  crashed_at_ms BIGINT,
  sequence BIGINT DEFAULT 0,
  source TEXT DEFAULT 'live',
  created_at TIMESTAMPTZ DEFAULT NOW()
);
CREATE INDEX IF NOT EXISTS idx_crash_rounds_crashed ON crash_rounds(crashed_at_ms DESC);
CREATE INDEX IF NOT EXISTS idx_crash_rounds_mult ON crash_rounds(multiplier);
