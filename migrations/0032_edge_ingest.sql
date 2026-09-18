
CREATE TABLE IF NOT EXISTS edge_ingest_events (
  id BIGSERIAL PRIMARY KEY,
  source TEXT,
  payload TEXT,
  created_at_ms BIGINT NOT NULL
);
