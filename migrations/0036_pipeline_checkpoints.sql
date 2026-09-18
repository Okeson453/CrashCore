
CREATE TABLE IF NOT EXISTS pipeline_checkpoints (
  name TEXT PRIMARY KEY,
  last_game_id TEXT,
  last_sequence BIGINT,
  updated_at_ms BIGINT NOT NULL
);
