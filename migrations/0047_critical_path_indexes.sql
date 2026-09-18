-- Critical-path scan indexes
CREATE INDEX IF NOT EXISTS idx_predictions_round_decision ON predictions (round_id, decision);
CREATE INDEX IF NOT EXISTS idx_outcomes_round_result ON outcomes (round_id, result);
CREATE INDEX IF NOT EXISTS idx_outbox_prediction ON notification_outbox (prediction_id);
CREATE INDEX IF NOT EXISTS idx_feedback_created ON feedback_events (created_at_ms DESC);
