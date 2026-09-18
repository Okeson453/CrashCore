"use client";

import {
  ResponsiveContainer,
  BarChart,
  Bar,
  XAxis,
  YAxis,
  Tooltip,
  CartesianGrid,
  Legend,
} from "recharts";
import type { LatencyStage } from "@/types/statistics";
import { fmtMs } from "@/lib/formatting/numbers";

interface CustomTooltipProps {
  active?: boolean;
  payload?: Array<{
    name: string;
    value: number;
    color: string;
  }>;
  label?: string;
}

function CustomTooltip({ active, payload, label }: CustomTooltipProps) {
  if (!active || !payload || !payload.length) return null;

  return (
    <div className="rounded-lg border border-surface-border bg-surface-panel/95 p-3 shadow-xl backdrop-blur-md text-xs font-mono">
      <div className="text-[11px] text-ink-dim mb-1.5 font-sans border-b border-surface-borderSubtle pb-1">
        Stage: <span className="text-ink font-semibold">{label}</span>
      </div>
      <div className="space-y-1">
        {payload.map((item, i) => (
          <div key={i} className="flex items-center justify-between gap-4">
            <span className="flex items-center gap-1.5 text-ink-muted">
              <span className="h-2 w-2 rounded-full" style={{ background: item.color }} />
              <span>{item.name}:</span>
            </span>
            <span className="font-semibold text-ink tabular-nums">{fmtMs(item.value)}</span>
          </div>
        ))}
      </div>
    </div>
  );
}

export function LatencyStageChart({
  stages = [],
  height = 280,
}: {
  stages: LatencyStage[];
  height?: number;
}) {
  const data = stages.map((s) => ({
    stage: s.stage.replace("Event ", "").replace(" compute", ""),
    p50: s.p50,
    p95: s.p95,
    p99: s.p99,
  }));

  return (
    <div style={{ height }} className="w-full">
      <ResponsiveContainer width="100%" height="100%">
        <BarChart data={data} margin={{ top: 12, right: 12, left: -10, bottom: 0 }}>
          <CartesianGrid stroke="#181f2c" strokeDasharray="3 3" vertical={false} />
          <XAxis
            dataKey="stage"
            tick={{ fill: "#64748b", fontSize: 10, fontFamily: "monospace" }}
            axisLine={{ stroke: "#222a3a" }}
            tickLine={{ stroke: "#222a3a" }}
          />
          <YAxis
            tick={{ fill: "#64748b", fontSize: 10, fontFamily: "monospace" }}
            width={44}
            axisLine={{ stroke: "#222a3a" }}
            tickLine={{ stroke: "#222a3a" }}
            unit="ms"
          />
          <Tooltip content={<CustomTooltip />} />
          <Legend
            wrapperStyle={{ fontSize: 11, fontFamily: "monospace", paddingTop: 8 }}
            iconType="circle"
          />
          <Bar dataKey="p50" name="p50 (Median)" fill="#06b6d4" radius={[2, 2, 0, 0]} maxBarSize={20} />
          <Bar dataKey="p95" name="p95" fill="#f59e0b" radius={[2, 2, 0, 0]} maxBarSize={20} />
          <Bar dataKey="p99" name="p99 (Tail)" fill="#f43f5e" radius={[2, 2, 0, 0]} maxBarSize={20} />
        </BarChart>
      </ResponsiveContainer>
    </div>
  );
}
