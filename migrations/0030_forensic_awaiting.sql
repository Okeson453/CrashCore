
CREATE TABLE IF NOT EXISTS forensic_awaiting (
  id BIGSERIAL PRIMARY KEY,
  prediction_id TEXT,
  round_id TEXT,
  reason TEXT,
  status TEXT DEFAULT 'open',
  created_at_ms BIGINT NOT NULL,
  resolved_at_ms BIGINT
);
CREATE INDEX IF NOT EXISTS idx_forensic_status ON forensic_awaiting(status);
