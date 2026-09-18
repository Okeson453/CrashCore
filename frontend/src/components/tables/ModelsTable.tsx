"use client";

import type { ModelStatsRow } from "@/types/statistics";
import { fmtPct, fmtNum, fmtMs } from "@/lib/formatting/numbers";
import clsx from "clsx";
import { Cpu, CheckCircle, HelpCircle } from "lucide-react";

export function ModelsTable({ rows }: { rows: ModelStatsRow[] }) {
  // Sort rows by win rate descending
  const sorted = [...rows].sort((a, b) => (b.winRate ?? 0) - (a.winRate ?? 0));

  return (
    <div className="overflow-x-auto rounded-lg border border-surface-border bg-surface-panel shadow-sm">
      <table className="w-full min-w-[900px] text-left text-xs">
        <thead className="border-b border-surface-border bg-surface-raised/80 font-mono text-[11px] text-ink-dim uppercase tracking-wider">
          <tr>
            <th className="px-3 py-2.5 font-medium">Model Architecture</th>
            <th className="px-3 py-2.5 font-medium">Status</th>
            <th className="px-3 py-2.5 font-medium">Sample Size</th>
            <th className="px-3 py-2.5 font-medium">Win Rate</th>
            <th className="px-3 py-2.5 font-medium">Record (W / L)</th>
            <th className="px-3 py-2.5 font-medium">Avg Edge</th>
            <th className="px-3 py-2.5 font-medium">Avg Conf</th>
            <th className="px-3 py-2.5 font-medium">Calib Error</th>
            <th className="px-3 py-2.5 font-medium">Max L-Streak</th>
            <th className="px-3 py-2.5 font-medium text-right">Avg Latency</th>
          </tr>
        </thead>
        <tbody className="divide-y divide-surface-borderSubtle">
          {sorted.map((r, index) => {
            const isTop = index === 0;
            return (
              <tr
                key={r.model}
                className="transition-colors hover:bg-surface-raised/50"
              >
                {/* Model Name & Desc */}
                <td className="px-3 py-2.5">
                  <div className="flex items-center gap-2">
                    <Cpu className="h-3.5 w-3.5 text-accent-cyan shrink-0" />
                    <div>
                      <div className="flex items-center gap-1.5 font-semibold text-ink">
                        <span>{r.model}</span>
                        {isTop && (
                          <span className="rounded bg-accent-cyan/15 px-1 py-0.2 text-[9px] font-mono text-accent-cyan border border-accent-cyan/30">
                            LEADER
                          </span>
                        )}
                      </div>
                      {r.description && (
                        <div className="text-[10px] text-ink-dim line-clamp-1">
                          {r.description}
                        </div>
                      )}
                    </div>
                  </div>
                </td>

                {/* Status */}
                <td className="px-3 py-2.5 font-mono">
                  <span className="inline-flex items-center gap-1 rounded bg-accent-emerald/10 px-2 py-0.5 text-[10px] font-semibold text-accent-emerald border border-accent-emerald/30">
                    <span className="h-1.5 w-1.5 rounded-full bg-accent-emerald" />
                    <span>{r.status ?? "ACTIVE"}</span>
                  </span>
                </td>

                {/* Sample Size */}
                <td className="px-3 py-2.5 font-mono tabular-nums text-ink-muted">
                  {fmtNum(r.sampleSize ?? r.predictions)}
                </td>

                {/* Win Rate with Bar */}
                <td className="px-3 py-2.5 font-mono tabular-nums">
                  <div className="w-24 space-y-1">
                    <div className="font-bold text-accent-emerald text-[12px]">
                      {fmtPct(r.winRate, 1)}
                    </div>
                    <div className="h-1.5 w-full rounded-full bg-surface-subtle overflow-hidden">
                      <div
                        className="h-full rounded-full bg-accent-emerald"
                        style={{ width: `${Math.min(100, r.winRate * 100)}%` }}
                      />
                    </div>
                  </div>
                </td>

                {/* Record W / L */}
                <td className="px-3 py-2.5 font-mono tabular-nums">
                  <span className="font-semibold text-accent-emerald">{fmtNum(r.wins)}</span>
                  <span className="text-ink-dim mx-1">/</span>
                  <span className="text-accent-rose">{fmtNum(r.losses)}</span>
                </td>

                {/* Avg Edge */}
                <td className="px-3 py-2.5 font-mono tabular-nums">
                  <span className="font-semibold text-accent-cyan">
                    +{fmtPct(r.avgEdge, 1)}
                  </span>
                </td>

                {/* Avg Conf */}
                <td className="px-3 py-2.5 font-mono tabular-nums text-ink-muted">
                  {fmtPct(r.avgConfidence, 1)}
                </td>

                {/* Calibration Error */}
                <td className="px-3 py-2.5 font-mono tabular-nums text-ink-muted">
                  {fmtPct(r.calibrationError, 2)}
                </td>

                {/* Max Losing Streak */}
                <td className="px-3 py-2.5 font-mono tabular-nums">
                  <span
                    className={clsx(
                      "font-semibold",
                      r.maxLosingStreak > 6 ? "text-accent-rose" : "text-ink-muted"
                    )}
                  >
                    {r.maxLosingStreak}
                  </span>
                </td>

                {/* Avg Latency */}
                <td className="px-3 py-2.5 text-right font-mono tabular-nums text-ink-dim">
                  {r.avgLatencyMs ? fmtMs(r.avgLatencyMs) : "—"}
                </td>
              </tr>
            );
          })}
        </tbody>
      </table>
    </div>
  );
}
