-- ACIE online state persistence (edge, weights snapshot)
CREATE TABLE IF NOT EXISTS acie_online_state (
  id TEXT PRIMARY KEY DEFAULT 'default',
  adaptive_edge DOUBLE PRECISION NOT NULL DEFAULT 0.02,
  samples_seen BIGINT DEFAULT 0,
  wins BIGINT DEFAULT 0,
  losses BIGINT DEFAULT 0,
  weights_json TEXT,
  updated_at_ms BIGINT NOT NULL
);
INSERT INTO acie_online_state (id, adaptive_edge, updated_at_ms)
VALUES ('default', 0.02, 0)
ON CONFLICT (id) DO NOTHING;
