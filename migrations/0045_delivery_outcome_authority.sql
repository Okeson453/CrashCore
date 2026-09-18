-- Delivery outcome authority (WIN/LOSS delivery tracking)
CREATE TABLE IF NOT EXISTS delivery_outcomes (
  prediction_id TEXT PRIMARY KEY,
  round_id TEXT,
  delivered BOOLEAN DEFAULT FALSE,
  delivered_at_ms BIGINT,
  telegram_message_id TEXT,
  error TEXT
);
CREATE INDEX IF NOT EXISTS idx_delivery_outcomes_round ON delivery_outcomes (round_id);
