-- Enforce unmatched target uniqueness already present via target_claims PK;
-- add partial unique for open predictions per round when status allows.
CREATE UNIQUE INDEX IF NOT EXISTS idx_predictions_open_target
  ON predictions (round_id)
  WHERE decision = 1; -- Enter only; one open enter per target round
