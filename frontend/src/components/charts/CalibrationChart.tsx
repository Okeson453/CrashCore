"use client";

import {
  ResponsiveContainer,
  ComposedChart,
  Line,
  Scatter,
  XAxis,
  YAxis,
  Tooltip,
  CartesianGrid,
} from "recharts";
import type { ReliabilityBin } from "@/types/statistics";
import { fmtPct, fmtNum } from "@/lib/formatting/numbers";

interface CustomTooltipProps {
  active?: boolean;
  payload?: Array<{
    value: number;
    payload: {
      bin: string;
      predictedPct: number;
      observedPct: number;
      count: number;
      error: number;
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
        Probability Bin: <span className="text-ink font-semibold">{p.bin}</span>
      </div>
      <div className="space-y-1">
        <div className="flex items-center justify-between gap-4">
          <span className="text-ink-muted">Predicted Avg:</span>
          <span className="text-accent-cyan tabular-nums">{fmtPct(p.predictedPct / 100)}</span>
        </div>
        <div className="flex items-center justify-between gap-4">
          <span className="text-ink-muted">Observed Win Rate:</span>
          <span className="font-semibold text-accent-emerald tabular-nums">{fmtPct(p.observedPct / 100)}</span>
        </div>
        <div className="flex items-center justify-between gap-4">
          <span className="text-ink-muted">Calibration Delta:</span>
          <span className="text-accent-amber tabular-nums">{fmtPct(p.error / 100)}</span>
        </div>
        <div className="flex items-center justify-between gap-4 border-t border-surface-borderSubtle pt-1 mt-1">
          <span className="text-ink-dim">Sample Count:</span>
          <span className="text-ink tabular-nums">{fmtNum(p.count)}</span>
        </div>
      </div>
    </div>
  );
}

export function CalibrationChart({
  data = [],
  height = 240,
}: {
  data?: ReliabilityBin[];
  height?: number;
}) {
  const points = data.map((d) => ({
    bin: d.bin,
    ideal: Number((d.predictedAvg * 100).toFixed(1)),
    predictedPct: Number((d.predictedAvg * 100).toFixed(1)),
    observedPct: Number((d.observedRate * 100).toFixed(1)),
    count: d.count,
    error: Math.abs(d.observedRate - d.predictedAvg) * 100,
  }));

  return (
    <div style={{ height }} className="w-full">
      <ResponsiveContainer width="100%" height="100%">
        <ComposedChart data={points} margin={{ top: 12, right: 12, left: -15, bottom: 0 }}>
          <CartesianGrid stroke="#181f2c" strokeDasharray="3 3" vertical={false} />
          <XAxis
            dataKey="bin"
            tick={{ fill: "#64748b", fontSize: 10, fontFamily: "monospace" }}
            axisLine={{ stroke: "#222a3a" }}
            tickLine={{ stroke: "#222a3a" }}
          />
          <YAxis
            tick={{ fill: "#64748b", fontSize: 10, fontFamily: "monospace" }}
            width={40}
            domain={[40, 100]}
            axisLine={{ stroke: "#222a3a" }}
            tickLine={{ stroke: "#222a3a" }}
            unit="%"
          />
          <Tooltip content={<CustomTooltip />} />
          {/* Ideal line */}
          <Line
            type="monotone"
            dataKey="ideal"
            stroke="#475569"
            strokeDasharray="4 4"
            strokeWidth={1.5}
            dot={false}
            name="Ideal Calibration"
          />
          {/* Actual observed points */}
          <Line
            type="monotone"
            dataKey="observedPct"
            stroke="#06b6d4"
            strokeWidth={2}
            dot={{ r: 4, fill: "#06b6d4", stroke: "#090a0f", strokeWidth: 1.5 }}
            name="Empirical Rate"
          />
        </ComposedChart>
      </ResponsiveContainer>
    </div>
  );
}
