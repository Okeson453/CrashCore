export function fmtPct(v: number, digits = 2): string {
  if (!Number.isFinite(v)) return "—";
  return `${(v * 100).toFixed(digits)}%`;
}

export function fmtNum(v: number, digits = 0): string {
  if (!Number.isFinite(v)) return "—";
  return v.toLocaleString(undefined, {
    minimumFractionDigits: digits,
    maximumFractionDigits: digits,
  });
}

export function fmtSignedNum(v: number, digits = 1): string {
  if (!Number.isFinite(v)) return "—";
  const sign = v > 0 ? "+" : "";
  return `${sign}${v.toLocaleString(undefined, {
    minimumFractionDigits: digits,
    maximumFractionDigits: digits,
  })}`;
}

export function fmtMs(v: number): string {
  if (!Number.isFinite(v)) return "—";
  if (v < 0.001) return `<1µs`;
  if (v < 1) return `${(v * 1000).toFixed(0)}µs`;
  if (v < 1000) return `${v.toFixed(1)}ms`;
  return `${(v / 1000).toFixed(2)}s`;
}

export function fmtMult(v: number): string {
  if (!Number.isFinite(v)) return "—";
  return `${v.toFixed(2)}x`;
}

export function fmtAge(ms: number): string {
  if (!Number.isFinite(ms) || ms < 0) return "—";
  if (ms < 1000) return `${Math.round(ms)} ms`;
  if (ms < 60_000) return `${(ms / 1000).toFixed(1)} s`;
  return `${Math.floor(ms / 60_000)}m ${Math.round((ms % 60_000) / 1000)}s`;
}

export function fmtTime(iso: string): string {
  try {
    const d = new Date(iso);
    return isNaN(d.getTime()) ? "—" : d.toLocaleTimeString(undefined, { hour12: false });
  } catch {
    return "—";
  }
}

export function fmtDateTime(iso: string): string {
  try {
    const d = new Date(iso);
    return isNaN(d.getTime()) ? "—" : d.toLocaleString(undefined, {
      month: "short",
      day: "numeric",
      hour: "2-digit",
      minute: "2-digit",
      second: "2-digit",
      hour12: false,
    });
  } catch {
    return "—";
  }
}
