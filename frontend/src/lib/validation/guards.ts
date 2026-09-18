/** Runtime guards for statistics payloads — presentation only. */

export function isFiniteNumber(v: unknown): v is number {
  return typeof v === "number" && Number.isFinite(v);
}

export function asNumber(v: unknown, fallback = 0): number {
  return isFiniteNumber(v) ? v : fallback;
}

export function asString(v: unknown, fallback = ""): string {
  return typeof v === "string" ? v : fallback;
}
