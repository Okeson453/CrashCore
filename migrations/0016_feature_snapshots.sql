
CREATE TABLE IF NOT EXISTS feature_snapshots (
  id BIGSERIAL PRIMARY KEY,
  round_id TEXT NOT NULL,
  feature_version TEXT,
  values_json TEXT NOT NULL,
  created_at_ms BIGINT NOT NULL
);
CREATE INDEX IF NOT EXISTS idx_feature_round ON feature_snapshots(round_id);
