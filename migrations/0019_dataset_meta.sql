
CREATE TABLE IF NOT EXISTS dataset_meta (
  id TEXT PRIMARY KEY,
  feature_version TEXT,
  target_version TEXT,
  sample_count INT,
  config_hash TEXT,
  leakage_check_passed BOOLEAN DEFAULT FALSE,
  generated_at_ms BIGINT
);
