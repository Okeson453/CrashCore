
CREATE TABLE IF NOT EXISTS worker_leases (
  lock_key TEXT PRIMARY KEY,
  owner_id TEXT NOT NULL,
  epoch BIGINT NOT NULL DEFAULT 1,
  expires_at_ms BIGINT NOT NULL
);
CREATE TABLE IF NOT EXISTS worker_state (
  worker_id TEXT PRIMARY KEY,
  state_json TEXT,
  updated_at_ms BIGINT
);
