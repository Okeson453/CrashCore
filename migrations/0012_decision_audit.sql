
CREATE TABLE IF NOT EXISTS decision_audit (
  id BIGSERIAL PRIMARY KEY,
  prediction_id TEXT,
  round_id TEXT,
  decision SMALLINT,
  probability DOUBLE PRECISION,
  confidence DOUBLE PRECISION,
  model_version TEXT,
  regime_id TEXT,
  features_json TEXT,
  reasoning TEXT,
  created_at_ms BIGINT NOT NULL
);
CREATE INDEX IF NOT EXISTS idx_decision_audit_round ON decision_audit(round_id);
