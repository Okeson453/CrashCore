"use client";

import clsx from "clsx";
import type { ConnectionState } from "@/types/statistics";
import { fmtAge } from "@/lib/formatting/numbers";

type Props = {
  state: ConnectionState;
  lastUpdate?: string | null;
  sequence?: number;
  dataAgeMs?: number;
};

const colors: Record<ConnectionState, string> = {
  LIVE: "bg-accent-live text-black",
  DEGRADED: "bg-accent-warning text-black",
  STALE: "bg-accent-warning/80 text-black",
  DISCONNECTED: "bg-accent-negative text-white",
};

export function ConnectionBadge({ state, lastUpdate, sequence, dataAgeMs }: Props) {
  const time =
    lastUpdate != null
      ? new Date(lastUpdate).toLocaleTimeString(undefined, { hour12: false })
      : "—";

  return (
    <div className="flex flex-wrap items-center gap-3 text-xs text-ink-muted">
      <span className={clsx("rounded px-2 py-0.5 font-semibold tracking-wide", colors[state])}>
        {state === "LIVE" ? "● LIVE" : state}
      </span>
      <span>
        Last update: <span className="font-mono text-ink">{time}</span>
      </span>
      {sequence != null ? (
        <span>
          Sequence: <span className="font-mono text-ink">#{sequence.toLocaleString()}</span>
        </span>
      ) : null}
      {dataAgeMs != null ? (
        <span>
          Data age: <span className="font-mono text-ink">{fmtAge(dataAgeMs)}</span>
        </span>
      ) : null}
    </div>
  );
}
