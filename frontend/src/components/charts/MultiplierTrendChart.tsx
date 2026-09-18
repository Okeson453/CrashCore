"use client";

import { useMemo, useState } from "react";
import {
  ResponsiveContainer,
  ComposedChart,
  Area,
  Line,
  XAxis,
  YAxis,
  Tooltip,
  CartesianGrid,
  ReferenceLine,
} from "recharts";
import type { RoundRow } from "@/types/statistics";
import { fmtDateTime, fmtMult, fmtTime } from "@/lib/formatting/numbers";
import { TrendingUp, Activity, Sparkles, SlidersHorizontal } from "lucide-react";
import clsx from "clsx";

interface Props {
  rounds: RoundRow[];
  height?: number;
  className?: string;
}

interface ChartPoint {
  index: number;
  gameId: string;
  rawTime: string;
  displayTime: string;
  multiplier: number;
  sma5: number;
  regime?: string;
  hash?: string;
  isMoon: boolean;
  isBust: boolean;
}

interface CustomTooltipProps {
  active?: boolean;
  payload?: Array<{
    value: number;
    dataKey: string;
    payload: ChartPoint;
  }>;
}

function CustomTooltip({ active, payload }: CustomTooltipProps) {
  if (!active || !payload || !payload.length) return null;
  const p = payload[0]?.payload;
  if (!p) return null;

  return (
    <div className="rounded-xl border border-surface-border bg-surface-panel/95 p-3.5 shadow-2xl backdrop-blur-md text-xs font-mono min-w-[200px]">
      <div className="flex items-center justify-between border-b border-surface-borderSubtle pb-2 mb-2">
        <span className="font-bold text-ink">{p.gameId}</span>
        <span className="text-[10px] text-ink-dim">{fmtDateTime(p.rawTime)}</span>
      </div>

      <div className="space-y-1.5">
        <div className="flex items-center justify-between gap-4">
          <span className="text-ink-muted font-sans text-[11px]">Crash Multiplier:</span>
          <span
            className={clsx(
              "font-bold tabular-nums text-sm",
              p.isMoon
                ? "text-amber-400"
                : p.isBust
                ? "text-accent-rose"
                : p.multiplier >= 2.0
                ? "text-accent-emerald"
                : "text-accent-cyan"
            )}
          >
            {fmtMult(p.multiplier)}
          </span>
        </div>

        <div className="flex items-center justify-between gap-4">
          <span className="text-ink-muted font-sans text-[11px]">5-Round SMA:</span>
          <span className="text-amber-400/90 font-medium tabular-nums">{fmtMult(p.sma5)}</span>
        </div>

        {p.regime && (
          <div className="flex items-center justify-between gap-4 pt-1 border-t border-surface-borderSubtle/60 text-[10px]">
            <span className="text-ink-muted font-sans">Regime:</span>
            <span className="text-ink-dim truncate max-w-[130px]">{p.regime}</span>
          </div>
        )}

        {p.hash && (
          <div className="flex items-center justify-between gap-2 pt-0.5 text-[9px] text-ink-dim">
            <span className="font-sans">SHA-256:</span>
            <span className="font-mono text-ink-muted truncate max-w-[120px]">
              {p.hash.slice(0, 10)}…{p.hash.slice(-6)}
            </span>
          </div>
        )}
      </div>
    </div>
  );
}

function CustomPointDot(props: any) {
  const { cx, cy, payload } = props;
  if (cx == null || cy == null || !payload) {
    return <circle r={0} cx={0} cy={0} />;
  }
  if (payload.isMoon) {
    return (
      <circle
        cx={cx}
        cy={cy}
        r={4.5}
        fill="#f59e0b"
        stroke="#ffffff"
        strokeWidth={1.5}
      />
    );
  }
  if (payload.isBust) {
    return (
      <circle
        cx={cx}
        cy={cy}
        r={2.5}
        fill="#f43f5e"
        stroke="#1e293b"
        strokeWidth={1}
      />
    );
  }
  return (
    <circle
      cx={cx}
      cy={cy}
      r={2}
      fill="#06b6d4"
      stroke="#0f172a"
      strokeWidth={1}
    />
  );
}

export function MultiplierTrendChart({ rounds, height = 280, className }: Props) {
  const [showSMA, setShowSMA] = useState(true);
  const [scaleMode, setScaleMode] = useState<"standard" | "clamped">("standard");

  // Transform data chronologically (oldest to newest for timeline evolution)
  const chartData = useMemo<ChartPoint[]>(() => {
    if (!rounds || rounds.length === 0) return [];

    // Sort ascending by timestamp or sequence
    const chronological = [...rounds].reverse();

    return chronological.map((r, i, arr) => {
      // Calculate 5-round simple moving average
      const windowStart = Math.max(0, i - 4);
      const slice = arr.slice(windowStart, i + 1);
      const sma5 = slice.reduce((sum, item) => sum + item.multiplier, 0) / slice.length;

      return {
        index: i + 1,
        gameId: r.gameId,
        rawTime: r.timestamp,
        displayTime: fmtTime(r.timestamp),
        multiplier: r.multiplier,
        sma5: Number(sma5.toFixed(2)),
        regime: r.regime,
        hash: r.hash,
        isMoon: r.multiplier >= 10.0,
        isBust: r.multiplier < 1.2,
      };
    });
  }, [rounds]);

  // Summary statistics for the current selection
  const stats = useMemo(() => {
    if (chartData.length === 0) return null;
    const mults = chartData.map((d) => d.multiplier);
    const sum = mults.reduce((a, b) => a + b, 0);
    const mean = sum / mults.length;
    const max = Math.max(...mults);
    const min = Math.min(...mults);
    
    // Median
    const sorted = [...mults].sort((a, b) => a - b);
    const mid = Math.floor(sorted.length / 2);
    const median = sorted.length % 2 !== 0 ? sorted[mid] : (sorted[mid - 1] + sorted[mid]) / 2;

    return { mean, max, min, median };
  }, [chartData]);

  if (chartData.length === 0) {
    return (
      <div
        className={clsx(
          "flex flex-col items-center justify-center rounded-xl border border-surface-border bg-surface-panel p-8 text-center",
          className
        )}
        style={{ height }}
      >
        <Activity className="h-8 w-8 text-ink-dim/40 mb-2 animate-pulse" />
        <p className="font-mono text-xs text-ink-dim">No historical round data available for selected filter</p>
      </div>
    );
  }

  // Determine domain bounds
  const yMax = scaleMode === "clamped" ? Math.min(stats?.max ?? 10, 15) : "auto";

  return (
    <div className={clsx("rounded-xl border border-surface-border bg-surface-panel p-4 shadow-sm space-y-3", className)}>
      {/* Chart Header & Controls */}
      <div className="flex flex-wrap items-center justify-between gap-3 border-b border-surface-borderSubtle pb-3">
        <div>
          <div className="flex items-center gap-2">
            <TrendingUp className="h-4 w-4 text-accent-cyan" />
            <h3 className="text-xs font-semibold text-ink font-mono uppercase tracking-wider">
              Multiplier Evolution & Trend Trajectory
            </h3>
            <span className="rounded bg-surface-raised px-1.5 py-0.5 font-mono text-[10px] text-ink-dim border border-surface-borderSubtle">
              {chartData.length} Data Points
            </span>
          </div>
          <p className="text-[11px] text-ink-dim mt-0.5">
            Chronological crash points, 5-round moving average, and baseline threshold marks
          </p>
        </div>

        {/* Action Controls */}
        <div className="flex items-center gap-2 font-mono text-xs">
          <button
            type="button"
            onClick={() => setShowSMA((s) => !s)}
            className={clsx(
              "flex items-center gap-1.5 rounded-md px-2.5 py-1 text-[11px] font-medium border transition-colors",
              showSMA
                ? "bg-amber-400/15 text-amber-300 border-amber-400/40"
                : "bg-surface-raised text-ink-dim border-surface-border hover:text-ink"
            )}
          >
            <span className="h-1.5 w-1.5 rounded-full bg-amber-400" />
            5-SMA Trend
          </button>

          <button
            type="button"
            onClick={() => setScaleMode((m) => (m === "standard" ? "clamped" : "standard"))}
            className={clsx(
              "flex items-center gap-1.5 rounded-md px-2.5 py-1 text-[11px] font-medium border transition-colors",
              scaleMode === "clamped"
                ? "bg-accent-cyan/15 text-accent-cyan border-accent-cyan/40"
                : "bg-surface-raised text-ink-dim border-surface-border hover:text-ink"
            )}
            title="Toggle Clamped View (caps ceiling at 15x to inspect lower-tier density)"
          >
            <SlidersHorizontal className="h-3 w-3" />
            {scaleMode === "clamped" ? "Clamped (≤15x)" : "Full Scale"}
          </button>
        </div>
      </div>

      {/* Metric ribbon above chart */}
      {stats && (
        <div className="grid grid-cols-2 gap-2 sm:grid-cols-4 font-mono text-xs bg-surface-raised/40 rounded-lg p-2 border border-surface-borderSubtle">
          <div className="flex items-center justify-between px-2">
            <span className="text-[10px] text-ink-dim uppercase">Range Max:</span>
            <span className="font-bold text-amber-400 tabular-nums">{fmtMult(stats.max)}</span>
          </div>
          <div className="flex items-center justify-between px-2 border-l border-surface-borderSubtle">
            <span className="text-[10px] text-ink-dim uppercase">Range Mean:</span>
            <span className="font-bold text-accent-cyan tabular-nums">{fmtMult(stats.mean)}</span>
          </div>
          <div className="flex items-center justify-between px-2 border-l border-surface-borderSubtle">
            <span className="text-[10px] text-ink-dim uppercase">Median:</span>
            <span className="font-bold text-ink tabular-nums">{fmtMult(stats.median)}</span>
          </div>
          <div className="flex items-center justify-between px-2 border-l border-surface-borderSubtle">
            <span className="text-[10px] text-ink-dim uppercase">Floor Min:</span>
            <span className="font-bold text-accent-rose tabular-nums">{fmtMult(stats.min)}</span>
          </div>
        </div>
      )}

      {/* Main Recharts Container */}
      <div style={{ width: "100%", height }}>
        <ResponsiveContainer width="100%" height="100%">
          <ComposedChart data={chartData} margin={{ top: 12, right: 16, left: -10, bottom: 4 }}>
            <defs>
              <linearGradient id="multGrad" x1="0" y1="0" x2="0" y2="1">
                <stop offset="0%" stopColor="var(--accent-cyan, #06b6d4)" stopOpacity={0.25} />
                <stop offset="100%" stopColor="var(--accent-cyan, #06b6d4)" stopOpacity={0.0} />
              </linearGradient>
            </defs>

            <CartesianGrid strokeDasharray="3 3" stroke="rgba(255,255,255,0.05)" vertical={false} />

            <XAxis
              dataKey="index"
              stroke="#64748b"
              fontSize={10}
              tickLine={false}
              axisLine={{ stroke: "rgba(255,255,255,0.1)" }}
              tickFormatter={(val) => `#${val}`}
            />

            <YAxis
              stroke="#64748b"
              fontSize={10}
              tickLine={false}
              axisLine={{ stroke: "rgba(255,255,255,0.1)" }}
              domain={[1, yMax]}
              tickFormatter={(val) => `${val.toFixed(1)}x`}
            />

            <Tooltip content={<CustomTooltip />} />

            {/* Threshold Reference Lines */}
            <ReferenceLine
              y={2.0}
              stroke="rgba(16, 185, 129, 0.35)"
              strokeDasharray="4 4"
              label={{
                value: "2.0x Parity",
                position: "insideTopLeft",
                fill: "#10b981",
                fontSize: 9,
                opacity: 0.6,
              }}
            />

            {stats?.mean && (
              <ReferenceLine
                y={stats.mean}
                stroke="rgba(6, 182, 212, 0.4)"
                strokeDasharray="2 2"
                label={{
                  value: `Mean ${fmtMult(stats.mean)}`,
                  position: "insideBottomRight",
                  fill: "#06b6d4",
                  fontSize: 9,
                  opacity: 0.7,
                }}
              />
            )}

            {/* Multiplier Filled Area under curve */}
            <Area
              type="monotone"
              dataKey="multiplier"
              fill="url(#multGrad)"
              stroke="transparent"
              isAnimationActive={false}
            />

            {/* Multiplier Main Line */}
            <Line
              type="monotone"
              dataKey="multiplier"
              stroke="#06b6d4"
              strokeWidth={1.8}
              dot={<CustomPointDot />}
              activeDot={{ r: 5, fill: "#38bdf8", stroke: "#fff", strokeWidth: 2 }}
              isAnimationActive={false}
            />

            {/* Optional 5-SMA Smoothed Trend Line */}
            {showSMA && (
              <Line
                type="monotone"
                dataKey="sma5"
                stroke="#f59e0b"
                strokeWidth={2}
                strokeDasharray="4 2"
                dot={false}
                activeDot={{ r: 4, fill: "#f59e0b" }}
                isAnimationActive={false}
              />
            )}
          </ComposedChart>
        </ResponsiveContainer>
      </div>

      {/* Legend & Guide Footer */}
      <div className="flex flex-wrap items-center justify-between gap-2 pt-1 border-t border-surface-borderSubtle text-[11px] font-mono text-ink-dim">
        <div className="flex items-center gap-4">
          <div className="flex items-center gap-1.5">
            <span className="h-2 w-2 rounded-full bg-accent-cyan" />
            <span>Crash Multiplier</span>
          </div>
          {showSMA && (
            <div className="flex items-center gap-1.5">
              <span className="h-1.5 w-3 rounded-full bg-amber-400" />
              <span>5-Round Moving Avg</span>
            </div>
          )}
          <div className="flex items-center gap-1.5">
            <span className="h-2 w-2 rounded-full bg-amber-400 ring-2 ring-amber-400/30" />
            <span>Moon (≥10x)</span>
          </div>
          <div className="flex items-center gap-1.5">
            <span className="h-2 w-2 rounded-full bg-accent-rose" />
            <span>Bust (&lt;1.2x)</span>
          </div>
        </div>

        <span className="text-[10px]">Sequence left-to-right (chronological)</span>
      </div>
    </div>
  );
}
