
CREATE TABLE IF NOT EXISTS signal_volume_daily (
  day DATE PRIMARY KEY,
  signals INT DEFAULT 0,
  actionable INT DEFAULT 0,
  wins INT DEFAULT 0,
  losses INT DEFAULT 0
);
