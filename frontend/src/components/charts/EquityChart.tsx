"use client";

import {
  ResponsiveContainer,
  AreaChart,
  Area,
  Line,
  XAxis,
  YAxis,
  Tooltip,
  CartesianGrid,
} from "recharts";
import type { PerformancePoint } from "@/types/statistics";
import { fmtDateTime } from "@/lib/formatting/numbers";

interface CustomTooltipProps {
  active?: boolean;
  payload?: Array<{
    value: number;
    dataKey: string;
    payload: {
      rawTime: string;
      equity: number;
      highWaterMark?: number;
      drawdown: number;
      winRate: number;
      pnlDelta?: number;
    };
  }>;
  label?: string;
}

function CustomTooltip({ active, payload }: CustomTooltipProps) {
  if (!active || !payload || !payload.length) return null;
  const p = payload[0]?.payload;
  if (!p) return null;

  return (
    <div className="rounded-lg border border-surface-border bg-surface-panel/95 p-3 shadow-xl backdrop-blur-md text-xs font-mono">
      <div className="text-[11px] text-ink-dim mb-1.5 font-sans border-b border-surface-borderSubtle pb-1">
        {fmtDateTime(p.rawTime)}
      </div>
      <div className="space-y-1">
        <div className="flex items-center justify-between gap-4">
          <span className="text-ink-muted">Equity Index:</span>
          <span className="font-semibold text-accent-cyan tabular-nums">{p.equity.toFixed(2)}</span>
        </div>
        {p.highWaterMark != null && (
          <div className="flex items-center justify-between gap-4">
            <span className="text-ink-muted">High Water Mark:</span>
            <span className="text-accent-emerald tabular-nums">{p.highWaterMark.toFixed(2)}</span>
          </div>
        )}
        <div className="flex items-center justify-between gap-4">
          <span className="text-ink-muted">Drawdown:</span>
          <span className="text-accent-rose tabular-nums">{(p.drawdown * 100).toFixed(2)}%</span>
        </div>
        <div className="flex items-center justify-between gap-4">
          <span className="text-ink-muted">Window Win Rate:</span>
          <span className="text-ink tabular-nums">{(p.winRate * 100).toFixed(1)}%</span>
        </div>
      </div>
    </div>
  );
}

export function EquityChart({
  data,
  height = 280,
  showHighWater = true,
}: {
  data: PerformancePoint[];
  height?: number;
  showHighWater?: boolean;
}) {
  const points = data.map((d, index) => ({
    rawTime: d.t,
    timeLabel: index % 15 === 0 ? fmtDateTime(d.t).split(",")[0] : "",
    equity: d.equity,
    highWaterMark: d.highWaterMark ?? d.equity,
    drawdown: d.drawdown,
    winRate: d.winRate,
    pnlDelta: d.pnlDelta,
  }));

  const minVal = Math.min(...points.map((p) => p.equity)) * 0.95;
  const maxVal = Math.max(...points.map((p) => Math.max(p.equity, p.highWaterMark ?? 0))) * 1.05;

  return (
    <div style={{ height }} className="w-full">
      <ResponsiveContainer width="100%" height="100%">
        <AreaChart data={points} margin={{ top: 10, right: 12, left: -10, bottom: 0 }}>
          <defs>
            <linearGradient id="equityGradient" x1="0" y1="0" x2="0" y2="1">
              <stop offset="5%" stopColor="#06b6d4" stopOpacity={0.35} />
              <stop offset="95%" stopColor="#06b6d4" stopOpacity={0.0} />
            </linearGradient>
          </defs>
          <CartesianGrid stroke="#181f2c" strokeDasharray="3 3" vertical={false} />
          <XAxis
            dataKey="timeLabel"
            tick={{ fill: "#64748b", fontSize: 10, fontFamily: "monospace" }}
            axisLine={{ stroke: "#222a3a" }}
            tickLine={{ stroke: "#222a3a" }}
          />
          <YAxis
            tick={{ fill: "#64748b", fontSize: 10, fontFamily: "monospace" }}
            width={44}
            domain={[Number(minVal.toFixed(0)), Number(maxVal.toFixed(0))]}
            axisLine={{ stroke: "#222a3a" }}
            tickLine={{ stroke: "#222a3a" }}
          />
          <Tooltip content={<CustomTooltip />} />
          {showHighWater && (
            <Line
              type="stepAfter"
              dataKey="highWaterMark"
              stroke="#10b981"
              strokeWidth={1}
              strokeDasharray="4 4"
              dot={false}
              isAnimationActive={false}
            />
          )}
          <Area
            type="monotone"
            dataKey="equity"
            stroke="#06b6d4"
            strokeWidth={2}
            fillOpacity={1}
            fill="url(#equityGradient)"
            isAnimationActive={true}
          />
        </AreaChart>
      </ResponsiveContainer>
    </div>
  );
}
