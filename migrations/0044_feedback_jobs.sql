-- Exactly-once feedback job queue (durable complement to in-process set)
CREATE TABLE IF NOT EXISTS feedback_jobs (
  id BIGSERIAL PRIMARY KEY,
  prediction_id TEXT NOT NULL UNIQUE,
  round_id TEXT,
  outcome TEXT NOT NULL,
  status TEXT NOT NULL DEFAULT 'pending',
  attempts INT DEFAULT 0,
  created_at_ms BIGINT NOT NULL,
  processed_at_ms BIGINT
);
CREATE INDEX IF NOT EXISTS idx_feedback_jobs_status ON feedback_jobs (status);
