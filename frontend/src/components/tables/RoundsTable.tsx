"use client";

import type { RoundRow } from "@/types/statistics";
import { fmtMult } from "@/lib/formatting/numbers";

export function RoundsTable({ rows }: { rows: RoundRow[] }) {
  return (
    <div className="overflow-x-auto rounded border border-surface-border">
      <table className="w-full text-left text-xs">
        <thead className="bg-surface-raised text-ink-dim">
          <tr>
            {["Game ID", "Timestamp", "Multiplier", "Hash", "Regime"].map((h) => (
              <th key={h} className="px-2 py-2 font-medium">
                {h}
              </th>
            ))}
          </tr>
        </thead>
        <tbody>
          {rows.map((r) => (
            <tr key={r.gameId} className="border-t border-surface-border/60 hover:bg-surface-raised/40">
              <td className="px-2 py-1.5 font-mono tabular-nums">{r.gameId}</td>
              <td className="px-2 py-1.5 font-mono">
                {new Date(r.timestamp).toLocaleString(undefined, { hour12: false })}
              </td>
              <td className="px-2 py-1.5 font-mono tabular-nums">{fmtMult(r.multiplier)}</td>
              <td className="px-2 py-1.5 font-mono text-ink-muted">{r.hash ?? "—"}</td>
              <td className="px-2 py-1.5">{r.regime ?? "—"}</td>
            </tr>
          ))}
        </tbody>
      </table>
    </div>
  );
}
