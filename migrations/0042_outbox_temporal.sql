-- Temporal delivery columns for outbox
ALTER TABLE notification_outbox ADD COLUMN IF NOT EXISTS target_started_at_ms BIGINT;
ALTER TABLE notification_outbox ADD COLUMN IF NOT EXISTS rejected_reason TEXT;
ALTER TABLE notification_outbox ADD COLUMN IF NOT EXISTS priority SMALLINT DEFAULT 3;
CREATE INDEX IF NOT EXISTS idx_outbox_priority_pending
  ON notification_outbox (priority DESC, available_at_ms ASC)
  WHERE status = 'pending';
