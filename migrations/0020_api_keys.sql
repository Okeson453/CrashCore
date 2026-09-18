
CREATE TABLE IF NOT EXISTS api_keys (
  id TEXT PRIMARY KEY,
  user_id BIGINT REFERENCES users(id) ON DELETE CASCADE,
  key_hash TEXT NOT NULL,
  scope TEXT DEFAULT 'statistics:read',
  expires_at_ms BIGINT,
  created_at_ms BIGINT NOT NULL
);
