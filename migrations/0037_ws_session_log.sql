
CREATE TABLE IF NOT EXISTS ws_session_log (
  id BIGSERIAL PRIMARY KEY,
  event TEXT NOT NULL,
  detail TEXT,
  created_at_ms BIGINT NOT NULL
);
