-- Worker epoch / fencing support
ALTER TABLE worker_leases ADD COLUMN IF NOT EXISTS generation BIGINT DEFAULT 1;
ALTER TABLE worker_leases ADD COLUMN IF NOT EXISTS last_renew_ms BIGINT;
CREATE INDEX IF NOT EXISTS idx_worker_leases_expires ON worker_leases (expires_at_ms);
