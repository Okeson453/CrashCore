"use client";

import type { ModelStatsRow } from "@/types/statistics";
import { fmtPct, fmtNum } from "@/lib/formatting/numbers";

export function ModelsTable({ rows }: { rows: ModelStatsRow[] }) {
  return (
    <div className="overflow-x-auto rounded border border-surface-border">
      <table className="w-full min-w-[800px] text-left text-xs">
        <thead className="bg-surface-raised text-ink-dim">
          <tr>
            {[
              "Model",
              "Predictions",
              "Wins",
              "Losses",
              "Win Rate",
              "Avg Prob",
              "Avg Conf",
              "Avg Edge",
              "Calib Err",
              "Max Streak",
            ].map((h) => (
              <th key={h} className="px-2 py-2 font-medium">
                {h}
              </th>
            ))}
          </tr>
        </thead>
        <tbody>
          {rows.map((r) => (
            <tr key={r.model} className="border-t border-surface-border/60 hover:bg-surface-raised/40">
              <td className="px-2 py-1.5 text-ink">{r.model}</td>
              <td className="px-2 py-1.5 font-mono tabular-nums">{fmtNum(r.predictions)}</td>
              <td className="px-2 py-1.5 font-mono tabular-nums text-accent-positive">{fmtNum(r.wins)}</td>
              <td className="px-2 py-1.5 font-mono tabular-nums text-accent-negative">{fmtNum(r.losses)}</td>
              <td className="px-2 py-1.5 font-mono tabular-nums">{fmtPct(r.winRate)}</td>
              <td className="px-2 py-1.5 font-mono tabular-nums">{fmtPct(r.avgProbability)}</td>
              <td className="px-2 py-1.5 font-mono tabular-nums">{fmtPct(r.avgConfidence)}</td>
              <td className="px-2 py-1.5 font-mono tabular-nums">{fmtPct(r.avgEdge)}</td>
              <td className="px-2 py-1.5 font-mono tabular-nums">{fmtPct(r.calibrationError)}</td>
              <td className="px-2 py-1.5 font-mono tabular-nums">{r.maxLosingStreak}</td>
            </tr>
          ))}
        </tbody>
      </table>
    </div>
  );
}
