
CREATE TABLE IF NOT EXISTS target_claims (
  round_id TEXT PRIMARY KEY,
  prediction_id TEXT NOT NULL,
  claimed_at_ms BIGINT NOT NULL,
  expires_at_ms BIGINT
);
