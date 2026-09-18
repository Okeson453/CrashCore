-- Phase 3: exactly-once feedback + temporal/outbox hardening indexes
-- Aligns with TestingEngine later-schema intent (idempotent feedback).

CREATE UNIQUE INDEX IF NOT EXISTS idx_feedback_events_prediction_unique
  ON feedback_events (prediction_id);

CREATE INDEX IF NOT EXISTS idx_outbox_round_status
  ON notification_outbox (round_id, status);

CREATE INDEX IF NOT EXISTS idx_outbox_pending_available
  ON notification_outbox (status, available_at_ms)
  WHERE status = 'pending';

CREATE INDEX IF NOT EXISTS idx_target_claims_prediction
  ON target_claims (prediction_id);

-- Optional provenance columns for ACIE (nullable; safe additive)
ALTER TABLE predictions ADD COLUMN IF NOT EXISTS provenance TEXT;
ALTER TABLE predictions ADD COLUMN IF NOT EXISTS strategy_mode TEXT;
ALTER TABLE predictions ADD COLUMN IF NOT EXISTS required_probability DOUBLE PRECISION;
