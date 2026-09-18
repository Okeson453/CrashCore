
CREATE TABLE IF NOT EXISTS telegram_delivery_log (
  id BIGSERIAL PRIMARY KEY,
  outbox_id BIGINT,
  message_id BIGINT,
  ok BOOLEAN,
  error TEXT,
  created_at_ms BIGINT NOT NULL
);
