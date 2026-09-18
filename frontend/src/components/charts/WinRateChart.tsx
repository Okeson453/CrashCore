"use client";

import {
  ResponsiveContainer,
  BarChart,
  Bar,
  XAxis,
  YAxis,
  Tooltip,
  CartesianGrid,
  ReferenceLine,
  Cell,
} from "recharts";

interface CustomTooltipProps {
  active?: boolean;
  payload?: Array<{
    value: number;
    payload: {
      window: string;
      rate: number;
      delta: number;
    };
  }>;
}

function CustomTooltip({ active, payload }: CustomTooltipProps) {
  if (!active || !payload || !payload.length) return null;
  const p = payload[0]?.payload;
  if (!p) return null;

  return (
    <div className="rounded-lg border border-surface-border bg-surface-panel/95 p-3 shadow-xl backdrop-blur-md text-xs font-mono">
      <div className="text-[11px] text-ink-dim mb-1 font-sans">
        Rolling Window: <span className="text-ink font-semibold">{p.window} rounds</span>
      </div>
      <div className="flex items-center justify-between gap-4">
        <span className="text-ink-muted">Win Rate:</span>
        <span className="font-semibold text-accent-emerald tabular-nums">{p.rate.toFixed(2)}%</span>
      </div>
      <div className="flex items-center justify-between gap-4 mt-0.5">
        <span className="text-ink-muted">Edge vs Baseline (48%):</span>
        <span className="font-semibold text-accent-cyan tabular-nums">+{p.delta.toFixed(2)}%</span>
      </div>
    </div>
  );
}

export function WinRateChart({
  data,
  height = 240,
}: {
  data: { window: number; rate: number }[];
  height?: number;
}) {
  const points = data.map((d) => {
    const ratePct = Number((d.rate * 100).toFixed(2));
    return {
      window: `${d.window}`,
      rate: ratePct,
      delta: ratePct - 48.0,
    };
  });

  return (
    <div style={{ height }} className="w-full">
      <ResponsiveContainer width="100%" height="100%">
        <BarChart data={points} margin={{ top: 12, right: 12, left: -15, bottom: 0 }}>
          <CartesianGrid stroke="#181f2c" strokeDasharray="3 3" vertical={false} />
          <XAxis
            dataKey="window"
            tick={{ fill: "#64748b", fontSize: 10, fontFamily: "monospace" }}
            axisLine={{ stroke: "#222a3a" }}
            tickLine={{ stroke: "#222a3a" }}
          />
          <YAxis
            tick={{ fill: "#64748b", fontSize: 10, fontFamily: "monospace" }}
            width={40}
            domain={[0, 100]}
            axisLine={{ stroke: "#222a3a" }}
            tickLine={{ stroke: "#222a3a" }}
            unit="%"
          />
          <Tooltip content={<CustomTooltip />} />
          <ReferenceLine
            y={48}
            stroke="#f59e0b"
            strokeDasharray="4 4"
            label={{
              value: "Baseline 48%",
              fill: "#f59e0b",
              fontSize: 10,
              position: "top",
            }}
          />
          <Bar dataKey="rate" radius={[4, 4, 0, 0]} maxBarSize={48}>
            {points.map((entry, index) => (
              <Cell
                key={`cell-${index}`}
                fill={entry.rate >= 48 ? "#10b981" : "#f43f5e"}
                opacity={0.9}
              />
            ))}
          </Bar>
        </BarChart>
      </ResponsiveContainer>
    </div>
  );
}
