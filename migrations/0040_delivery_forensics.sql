-- Phase 3/4: delivery forensics persistence (optional durable trail)

CREATE TABLE IF NOT EXISTS delivery_forensics (
  id BIGSERIAL PRIMARY KEY,
  correlation_id TEXT NOT NULL,
  outbox_id TEXT,
  stage TEXT NOT NULL,
  error TEXT,
  created_at_ms BIGINT NOT NULL
);
CREATE INDEX IF NOT EXISTS idx_forensics_corr ON delivery_forensics (correlation_id);
CREATE INDEX IF NOT EXISTS idx_forensics_created ON delivery_forensics (created_at_ms DESC);
