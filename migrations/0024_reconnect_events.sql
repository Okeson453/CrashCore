
CREATE TABLE IF NOT EXISTS reconnect_events (
  id BIGSERIAL PRIMARY KEY,
  reason TEXT,
  attempt INT,
  success BOOLEAN,
  created_at_ms BIGINT NOT NULL
);
