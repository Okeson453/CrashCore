
CREATE TABLE IF NOT EXISTS notification_outbox (
  id BIGSERIAL PRIMARY KEY,
  state SMALLINT NOT NULL DEFAULT 0,
  status TEXT DEFAULT 'pending',
  prediction_id TEXT,
  round_id TEXT,
  signal_kind SMALLINT,
  text TEXT,
  payload TEXT,
  dedupe_key TEXT UNIQUE,
  attempts INT DEFAULT 0,
  last_error TEXT,
  worker_id TEXT,
  available_at_ms BIGINT DEFAULT 0,
  created_at_ms BIGINT,
  claimed_at_ms BIGINT,
  delivered_at_ms BIGINT,
  created_at TIMESTAMPTZ DEFAULT NOW()
);
CREATE INDEX IF NOT EXISTS idx_outbox_state ON notification_outbox(state);
CREATE INDEX IF NOT EXISTS idx_outbox_status ON notification_outbox(status);
CREATE INDEX IF NOT EXISTS idx_outbox_available ON notification_outbox(available_at_ms);
