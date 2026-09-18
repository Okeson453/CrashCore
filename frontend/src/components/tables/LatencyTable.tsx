"use client";

import type { LatencyStage } from "@/types/statistics";
import { fmtMs } from "@/lib/formatting/numbers";

export function LatencyTable({ stages }: { stages: LatencyStage[] }) {
  return (
    <div className="overflow-x-auto rounded border border-surface-border">
      <table className="w-full text-left text-xs">
        <thead className="bg-surface-raised text-ink-dim">
          <tr>
            {["Stage", "p50", "p95", "p99", "Max"].map((h) => (
              <th key={h} className="px-2 py-2 font-medium">
                {h}
              </th>
            ))}
          </tr>
        </thead>
        <tbody>
          {stages.map((s) => (
            <tr key={s.stage} className="border-t border-surface-border/60 hover:bg-surface-raised/40">
              <td className="px-2 py-1.5 text-ink">{s.stage}</td>
              <td className="px-2 py-1.5 font-mono tabular-nums">{fmtMs(s.p50)}</td>
              <td className="px-2 py-1.5 font-mono tabular-nums">{fmtMs(s.p95)}</td>
              <td className="px-2 py-1.5 font-mono tabular-nums">{fmtMs(s.p99)}</td>
              <td className="px-2 py-1.5 font-mono tabular-nums">{fmtMs(s.max)}</td>
            </tr>
          ))}
        </tbody>
      </table>
    </div>
  );
}
