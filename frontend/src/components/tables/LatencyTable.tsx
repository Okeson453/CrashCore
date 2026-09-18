"use client";

import type { LatencyStage } from "@/types/statistics";
import { fmtMs } from "@/lib/formatting/numbers";
import clsx from "clsx";
import { Timer, ArrowRight } from "lucide-react";

export function LatencyTable({ stages }: { stages: LatencyStage[] }) {
  const maxAcross = Math.max(...stages.map((s) => s.max || 1));

  return (
    <div className="overflow-x-auto rounded-lg border border-surface-border bg-surface-panel shadow-sm">
      <table className="w-full min-w-[760px] text-left text-xs">
        <thead className="border-b border-surface-border bg-surface-raised/80 font-mono text-[11px] text-ink-dim uppercase tracking-wider">
          <tr>
            <th className="px-3 py-2.5 font-medium">Pipeline Stage</th>
            <th className="px-3 py-2.5 font-medium">p50 (Median)</th>
            <th className="px-3 py-2.5 font-medium">p95</th>
            <th className="px-3 py-2.5 font-medium">p99 (Tail)</th>
            <th className="px-3 py-2.5 font-medium">Max Peak</th>
            <th className="px-3 py-2.5 font-medium text-right">Relative Latency Profile</th>
          </tr>
        </thead>
        <tbody className="divide-y divide-surface-borderSubtle">
          {stages.map((s, index) => {
            const p99Pct = (s.p99 / maxAcross) * 100;
            return (
              <tr key={s.stage} className="transition-colors hover:bg-surface-raised/50">
                {/* Pipeline Step */}
                <td className="px-3 py-2.5">
                  <div className="flex items-center gap-2">
                    <span className="flex h-5 w-5 items-center justify-center rounded bg-surface-subtle font-mono text-[10px] text-accent-cyan font-bold">
                      {index + 1}
                    </span>
                    <span className="font-semibold text-ink">{s.stage}</span>
                  </div>
                </td>

                {/* p50 */}
                <td className="px-3 py-2.5 font-mono tabular-nums text-accent-cyan font-medium">
                  {fmtMs(s.p50)}
                </td>

                {/* p95 */}
                <td className="px-3 py-2.5 font-mono tabular-nums text-ink-muted">
                  {fmtMs(s.p95)}
                </td>

                {/* p99 */}
                <td className="px-3 py-2.5 font-mono tabular-nums">
                  <span
                    className={clsx(
                      "font-semibold",
                      s.p99 < 15
                        ? "text-accent-emerald"
                        : s.p99 < 40
                        ? "text-accent-amber"
                        : "text-accent-rose"
                    )}
                  >
                    {fmtMs(s.p99)}
                  </span>
                </td>

                {/* Max */}
                <td className="px-3 py-2.5 font-mono tabular-nums text-ink-dim">
                  {fmtMs(s.max)}
                </td>

                {/* Relative Bar */}
                <td className="px-3 py-2.5 text-right font-mono">
                  <div className="flex items-center justify-end gap-2">
                    <div className="h-2 w-32 rounded-full bg-surface-subtle overflow-hidden">
                      <div
                        className={clsx(
                          "h-full rounded-full",
                          s.p99 < 15
                            ? "bg-accent-emerald"
                            : s.p99 < 40
                            ? "bg-accent-amber"
                            : "bg-accent-rose"
                        )}
                        style={{ width: `${Math.max(6, Math.min(100, p99Pct))}%` }}
                      />
                    </div>
                  </div>
                </td>
              </tr>
            );
          })}
        </tbody>
      </table>
    </div>
  );
}
