
CREATE TABLE IF NOT EXISTS recent_rounds_cache (
  game_id TEXT PRIMARY KEY,
  multiplier DOUBLE PRECISION,
  crashed_at_ms BIGINT,
  ingested_at_ms BIGINT NOT NULL
);
