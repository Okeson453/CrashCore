
CREATE TABLE IF NOT EXISTS feedback_events (
  id BIGSERIAL PRIMARY KEY,
  prediction_id TEXT NOT NULL,
  round_id TEXT,
  outcome TEXT,
  actual_mult DOUBLE PRECISION,
  delivered BOOLEAN DEFAULT FALSE,
  created_at_ms BIGINT NOT NULL
);
CREATE INDEX IF NOT EXISTS idx_feedback_pred ON feedback_events(prediction_id);
