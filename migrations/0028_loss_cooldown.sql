
CREATE TABLE IF NOT EXISTS loss_cooldown_state (
  id TEXT PRIMARY KEY DEFAULT 'global',
  consecutive_losses INT DEFAULT 0,
  cooldown_until_ms BIGINT DEFAULT 0,
  updated_at_ms BIGINT NOT NULL
);
