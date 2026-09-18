#pragma once
/**
 * SQL schema fragments for CrashCore persistence.
 * Aligned with TestingEngine migrations for rounds, predictions, outcomes, outbox.
 */
#include <string>
#include <string_view>
#include <vector>

namespace crashcore::schema {

inline constexpr std::string_view CREATE_CRASH_ROUNDS = R"SQL(
CREATE TABLE IF NOT EXISTS crash_rounds (
  game_id       TEXT PRIMARY KEY,
  multiplier    DOUBLE PRECISION NOT NULL,
  hash          TEXT,
  salt          TEXT,
  began_at_ms   BIGINT,
  crashed_at_ms BIGINT,
  sequence      BIGINT DEFAULT 0,
  created_at    TIMESTAMPTZ DEFAULT NOW()
);
CREATE INDEX IF NOT EXISTS idx_crash_rounds_crashed ON crash_rounds(crashed_at_ms DESC);
)SQL";

inline constexpr std::string_view CREATE_PREDICTIONS = R"SQL(
CREATE TABLE IF NOT EXISTS predictions (
  id              TEXT PRIMARY KEY,
  round_id        TEXT NOT NULL,
  game_id         TEXT,
  decision        SMALLINT NOT NULL DEFAULT 0,
  confidence      DOUBLE PRECISION DEFAULT 0,
  entry_mult      DOUBLE PRECISION DEFAULT 0,
  target_mult     DOUBLE PRECISION DEFAULT 0,
  correlation_id  TEXT,
  model_version   TEXT,
  created_at_ms   BIGINT,
  created_at      TIMESTAMPTZ DEFAULT NOW()
);
CREATE INDEX IF NOT EXISTS idx_predictions_round ON predictions(round_id);
CREATE INDEX IF NOT EXISTS idx_predictions_created ON predictions(created_at_ms DESC);
)SQL";

inline constexpr std::string_view CREATE_OUTCOMES = R"SQL(
CREATE TABLE IF NOT EXISTS outcomes (
  prediction_id   TEXT PRIMARY KEY,
  round_id        TEXT NOT NULL,
  result          TEXT NOT NULL,
  actual_mult     DOUBLE PRECISION,
  target_mult     DOUBLE PRECISION,
  is_win          BOOLEAN DEFAULT FALSE,
  resolved_at_ms  BIGINT,
  created_at      TIMESTAMPTZ DEFAULT NOW()
);
CREATE INDEX IF NOT EXISTS idx_outcomes_round ON outcomes(round_id);
CREATE INDEX IF NOT EXISTS idx_outcomes_result ON outcomes(result);
)SQL";

inline constexpr std::string_view CREATE_OUTBOX = R"SQL(
CREATE TABLE IF NOT EXISTS notification_outbox (
  id              BIGSERIAL PRIMARY KEY,
  state           SMALLINT NOT NULL DEFAULT 0,
  prediction_id   TEXT,
  round_id        TEXT,
  signal_kind     SMALLINT,
  text            TEXT,
  dedupe_key      TEXT UNIQUE,
  attempts        INT DEFAULT 0,
  last_error      TEXT,
  created_at_ms   BIGINT,
  claimed_at_ms   BIGINT,
  delivered_at_ms BIGINT,
  created_at      TIMESTAMPTZ DEFAULT NOW()
);
CREATE INDEX IF NOT EXISTS idx_outbox_state ON notification_outbox(state);
CREATE INDEX IF NOT EXISTS idx_outbox_dedupe ON notification_outbox(dedupe_key);
)SQL";

inline std::vector<std::string_view> allMigrations() {
  return {
    CREATE_CRASH_ROUNDS,
    CREATE_PREDICTIONS,
    CREATE_OUTCOMES,
    CREATE_OUTBOX
  };
}

} // namespace crashcore::schema
