"use client";

import type { RegimeStatsRow } from "@/types/statistics";
import { fmtPct, fmtNum } from "@/lib/formatting/numbers";
import { Activity, TrendingUp, TrendingDown, RefreshCw, Zap } from "lucide-react";

function getRegimeIcon(regime: string) {
  const lower = regime.toLowerCase();
  if (lower.includes("trend") || lower.includes("bull") || lower.includes("high")) {
    return <TrendingUp className="h-3.5 w-3.5 text-accent-emerald" />;
  }
  if (lower.includes("revert") || lower.includes("mean") || lower.includes("choppy")) {
    return <RefreshCw className="h-3.5 w-3.5 text-accent-cyan" />;
  }
  if (lower.includes("volatil") || lower.includes("turbul")) {
    return <Zap className="h-3.5 w-3.5 text-accent-amber" />;
  }
  return <Activity className="h-3.5 w-3.5 text-ink-dim" />;
}

export function RegimesTable({ rows }: { rows: RegimeStatsRow[] }) {
  const sorted = [...rows].sort((a, b) => (b.winRate ?? 0) - (a.winRate ?? 0));
  const totalSamples = rows.reduce((acc, r) => acc + (r.samples || 0), 0) || 1;

  return (
    <div className="overflow-x-auto rounded-lg border border-surface-border bg-surface-panel shadow-sm">
      <table className="w-full min-w-[760px] text-left text-xs">
        <thead className="border-b border-surface-border bg-surface-raised/80 font-mono text-[11px] text-ink-dim uppercase tracking-wider">
          <tr>
            <th className="px-3 py-2.5 font-medium">Market Regime</th>
            <th className="px-3 py-2.5 font-medium">Distribution</th>
            <th className="px-3 py-2.5 font-medium">Predictions</th>
            <th className="px-3 py-2.5 font-medium">Win Rate</th>
            <th className="px-3 py-2.5 font-medium">Record (W / L)</th>
            <th className="px-3 py-2.5 font-medium">Avg Edge</th>
            <th className="px-3 py-2.5 font-medium">Avg Prob</th>
            <th className="px-3 py-2.5 font-medium text-right">Avg Conf</th>
          </tr>
        </thead>
        <tbody className="divide-y divide-surface-borderSubtle">
          {sorted.map((r) => {
            const share = r.samples / totalSamples;
            return (
              <tr key={r.regime} className="transition-colors hover:bg-surface-raised/50">
                {/* Regime Name & Icon */}
                <td className="px-3 py-2.5">
                  <div className="flex items-center gap-2">
                    {getRegimeIcon(r.regime)}
                    <span className="font-semibold text-ink">{r.regime}</span>
                  </div>
                </td>

                {/* Sample share */}
                <td className="px-3 py-2.5 font-mono tabular-nums text-ink-muted">
                  <div className="flex items-center gap-2">
                    <div className="h-1.5 w-16 rounded-full bg-surface-subtle overflow-hidden">
                      <div
                        className="h-full rounded-full bg-accent-cyan"
                        style={{ width: `${Math.min(100, share * 100)}%` }}
                      />
                    </div>
                    <span className="text-[11px]">{fmtPct(share, 0)}</span>
                  </div>
                </td>

                {/* Predictions */}
                <td className="px-3 py-2.5 font-mono tabular-nums text-ink-muted">
                  {fmtNum(r.predictions)}
                </td>

                {/* Win Rate */}
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

                {/* Record */}
                <td className="px-3 py-2.5 font-mono tabular-nums">
                  <span className="font-semibold text-accent-emerald">{fmtNum(r.wins)}</span>
                  <span className="text-ink-dim mx-1">/</span>
                  <span className="text-accent-rose">{fmtNum(r.losses)}</span>
                </td>

                {/* Avg Edge */}
                <td className="px-3 py-2.5 font-mono tabular-nums">
                  <span className="font-semibold text-accent-cyan">
                    +{fmtPct(Math.max(0, r.avgProbability - 0.48), 1)}
                  </span>
                </td>

                {/* Avg Prob */}
                <td className="px-3 py-2.5 font-mono tabular-nums text-ink-muted">
                  {fmtPct(r.avgProbability, 1)}
                </td>

                {/* Avg Conf */}
                <td className="px-3 py-2.5 text-right font-mono tabular-nums text-ink-muted">
                  {fmtPct(r.avgConfidence, 1)}
                </td>
              </tr>
            );
          })}
        </tbody>
      </table>
    </div>
  );
}
