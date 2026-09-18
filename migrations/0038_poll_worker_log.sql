
CREATE TABLE IF NOT EXISTS poll_worker_log (
  id BIGSERIAL PRIMARY KEY,
  pages INT,
  rounds INT,
  errors INT,
  created_at_ms BIGINT NOT NULL
);
