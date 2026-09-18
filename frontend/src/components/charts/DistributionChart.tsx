"use client";

import { ResponsiveContainer, PieChart, Pie, Cell, Tooltip } from "recharts";
import { fmtNum, fmtPct } from "@/lib/formatting/numbers";

const COLORS = [
  "#10b981", // Wins - Emerald
  "#f43f5e", // Losses - Rose
  "#64748b", // Skips - Dim Slate
];

interface CustomTooltipProps {
  active?: boolean;
  payload?: Array<{
    name: string;
    value: number;
    payload: {
      name: string;
      value: number;
      pct: number;
    };
  }>;
}

function CustomTooltip({ active, payload }: CustomTooltipProps) {
  if (!active || !payload || !payload.length) return null;
  const p = payload[0];
  if (!p) return null;

  return (
    <div className="rounded-lg border border-surface-border bg-surface-panel/95 p-2.5 shadow-xl backdrop-blur-md text-xs font-mono">
      <div className="flex items-center justify-between gap-4">
        <span className="text-ink-muted">{p.name}:</span>
        <span className="font-semibold text-ink tabular-nums">{fmtNum(p.value)}</span>
      </div>
      <div className="flex items-center justify-between gap-4 mt-0.5">
        <span className="text-ink-dim">Proportion:</span>
        <span className="font-semibold text-accent-cyan tabular-nums">{fmtPct(p.payload.pct)}</span>
      </div>
    </div>
  );
}

export function DistributionChart({
  wins,
  losses,
  skips,
  height = 240,
}: {
  wins: number;
  losses: number;
  skips: number;
  height?: number;
}) {
  const total = wins + losses + skips || 1;
  const data = [
    { name: "Wins", value: wins, pct: wins / total },
    { name: "Losses", value: losses, pct: losses / total },
    { name: "Skips", value: skips, pct: skips / total },
  ];

  return (
    <div className="flex flex-col items-center justify-center">
      <div style={{ height }} className="w-full relative">
        <ResponsiveContainer width="100%" height="100%">
          <PieChart>
            <Pie
              data={data}
              dataKey="value"
              nameKey="name"
              innerRadius={55}
              outerRadius={85}
              paddingAngle={3}
              stroke="#090a0f"
              strokeWidth={2}
            >
              {data.map((_, i) => (
                <Cell key={i} fill={COLORS[i % COLORS.length]} />
              ))}
            </Pie>
            <Tooltip content={<CustomTooltip />} />
          </PieChart>
        </ResponsiveContainer>
        {/* Center count info */}
        <div className="absolute inset-0 flex flex-col items-center justify-center pointer-events-none">
          <span className="text-[10px] font-mono uppercase text-ink-dim">Total</span>
          <span className="font-mono text-base font-bold text-ink tabular-nums">{fmtNum(total)}</span>
        </div>
      </div>

      {/* Custom Legend */}
      <div className="grid grid-cols-3 gap-2 w-full pt-2 border-t border-surface-borderSubtle text-xs font-mono">
        <div className="flex items-center gap-1.5">
          <span className="h-2 w-2 rounded-full bg-accent-emerald shrink-0" />
          <div className="truncate">
            <div className="text-[10px] text-ink-dim">Wins</div>
            <div className="text-ink font-semibold">{fmtPct(wins / total, 1)}</div>
          </div>
        </div>
        <div className="flex items-center gap-1.5">
          <span className="h-2 w-2 rounded-full bg-accent-rose shrink-0" />
          <div className="truncate">
            <div className="text-[10px] text-ink-dim">Losses</div>
            <div className="text-ink font-semibold">{fmtPct(losses / total, 1)}</div>
          </div>
        </div>
        <div className="flex items-center gap-1.5">
          <span className="h-2 w-2 rounded-full bg-slate-500 shrink-0" />
          <div className="truncate">
            <div className="text-[10px] text-ink-dim">Skips</div>
            <div className="text-ink font-semibold">{fmtPct(skips / total, 1)}</div>
          </div>
        </div>
      </div>
    </div>
  );
}
