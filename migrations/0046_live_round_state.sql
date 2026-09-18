-- Live round lifecycle state for recovery / invariants
CREATE TABLE IF NOT EXISTS live_round_state (
  round_id TEXT PRIMARY KEY,
  game_id TEXT,
  state TEXT NOT NULL DEFAULT 'unknown',
  began_at_ms BIGINT,
  ended_at_ms BIGINT,
  crash_point DOUBLE PRECISION,
  updated_at_ms BIGINT NOT NULL
);
CREATE INDEX IF NOT EXISTS idx_live_round_state_updated ON live_round_state (updated_at_ms DESC);
