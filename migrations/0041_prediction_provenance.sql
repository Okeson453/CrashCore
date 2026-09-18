-- TestingEngine-aligned prediction provenance / strategy audit
ALTER TABLE predictions ADD COLUMN IF NOT EXISTS strategy_action TEXT;
ALTER TABLE predictions ADD COLUMN IF NOT EXISTS fair_probability DOUBLE PRECISION;
ALTER TABLE predictions ADD COLUMN IF NOT EXISTS edge_used DOUBLE PRECISION;
ALTER TABLE predictions ADD COLUMN IF NOT EXISTS samples_seen BIGINT;
CREATE INDEX IF NOT EXISTS idx_predictions_strategy ON predictions (strategy_action);
