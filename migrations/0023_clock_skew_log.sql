
CREATE TABLE IF NOT EXISTS clock_skew_log (
  id BIGSERIAL PRIMARY KEY,
  offset_ms BIGINT,
  rtt_ms BIGINT,
  created_at_ms BIGINT NOT NULL
);
