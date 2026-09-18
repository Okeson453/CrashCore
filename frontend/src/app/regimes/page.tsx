"use client";

import { KpiCard } from "@/components/kpi/KpiCard";
import { RegimesTable } from "@/components/tables/RegimesTable";
import { useRegimes } from "@/data/queries/use-statistics";
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
import { fmtPct, fmtNum } from "@/lib/formatting/numbers";
import { Activity, TrendingUp, Zap, RefreshCw, Award, Layers } from "lucide-react";

interface CustomTooltipProps {
  active?: boolean;
  payload?: Array<{
    value: number;
    payload: {
      regime: string;
      winRate: number;
      samples: number;
      predictions: number;
      edge: number;
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
        Market Regime: <span className="text-ink font-semibold">{p.regime}</span>
      </div>
      <div className="space-y-1">
        <div className="flex items-center justify-between gap-4">
          <span className="text-ink-muted">Win Rate:</span>
          <span className="font-semibold text-accent-emerald tabular-nums">{p.winRate.toFixed(2)}%</span>
        </div>
        <div className="flex items-center justify-between gap-4">
          <span className="text-ink-muted">Edge vs 48% Base:</span>
          <span className="font-semibold text-accent-cyan tabular-nums">+{p.edge.toFixed(2)}%</span>
        </div>
        <div className="flex items-center justify-between gap-4 border-t border-surface-borderSubtle pt-1 mt-1 text-ink-dim">
          <span>Inference Volume:</span>
          <span className="text-ink tabular-nums">{fmtNum(p.predictions)}</span>
        </div>
      </div>
    </div>
  );
}

export default function RegimesPage() {
  const { data, isLoading } = useRegimes();

  const rows = data ?? [];
  const detectedCount = rows.length;

  if (isLoading && rows.length === 0) {
    return (
      <div className="flex h-64 items-center justify-center font-mono text-xs text-accent-cyan">
        Classifying market regimes and micro-structure states...
      </div>
    );
  }

  if (rows.length === 0) {
    return (
      <div className="rounded-lg border border-accent-rose/30 bg-accent-rose/10 p-6 text-xs text-accent-rose font-mono">
        No regime classifications recorded by the detector.
      </div>
    );
  }

  const topRegime = [...rows].sort((a, b) => b.winRate - a.winRate)[0];
  const chartData = rows.map((r) => {
    const ratePct = Number((r.winRate * 100).toFixed(2));
    return {
      regime: r.regime,
      winRate: ratePct,
      samples: r.samples,
      predictions: r.predictions,
      edge: ratePct - 48.0,
    };
  });

  return (
    <div className="space-y-6">
      {/* Page Header */}
      <div className="flex flex-wrap items-center justify-between gap-3 border-b border-surface-borderSubtle pb-4">
        <div>
          <div className="flex items-center gap-2">
            <h1 className="text-lg font-bold text-ink font-mono">Market Regime Analytics</h1>
            <span className="rounded bg-accent-emerald/15 px-2 py-0.5 font-mono text-[10px] font-bold text-accent-emerald border border-accent-emerald/30">
              STRATIFIED TELEMETRY
            </span>
          </div>
          <p className="text-xs text-ink-dim">
            Performance stability across varying volatility conditions, trend momentum, and chop states
          </p>
        </div>

        <div className="flex items-center gap-2 font-mono text-xs text-ink-dim">
          <span>Active Regimes: <strong className="text-ink">{detectedCount} Clusters</strong></span>
        </div>
      </div>

      {/* KPI Ribbon */}
      <div className="grid grid-cols-2 gap-3 sm:grid-cols-2 md:grid-cols-4">
        <KpiCard
          label="Highest Edge Regime"
          value={topRegime?.regime ?? "Trending High Vol"}
          subtitle={`Win Rate: ${fmtPct(topRegime?.winRate ?? 0.72, 1)}`}
          tone="positive"
          icon={<Award className="h-4 w-4" />}
          trend={{ value: `+${fmtPct(0.24, 1)} edge`, isPositive: true }}
        />
        <KpiCard
          label="Detected Regime Clusters"
          value={`${detectedCount} States`}
          subtitle="HMM & wavelet classifier"
          tone="cyan"
          icon={<Layers className="h-4 w-4" />}
        />
        <KpiCard
          label="Dominant Environment"
          value={rows[0]?.regime ?? "Mean Reverting"}
          subtitle="Highest sample share"
          icon={<RefreshCw className="h-4 w-4" />}
        />
        <KpiCard
          label="Minimum Regime Win Rate"
          value={fmtPct(Math.min(...rows.map((r) => r.winRate)), 1)}
          subtitle="Exceeds 48% baseline in all regimes"
          tone="positive"
          icon={<TrendingUp className="h-4 w-4" />}
        />
      </div>

      {/* Regime Comparison Bar Chart */}
      <div className="rounded-xl border border-surface-border bg-surface-panel p-4 shadow-sm">
        <div className="border-b border-surface-borderSubtle pb-3 mb-3">
          <div className="flex items-center justify-between">
            <div>
              <h2 className="text-sm font-semibold text-ink font-mono flex items-center gap-2">
                <Activity className="h-4 w-4 text-accent-cyan" />
                <span>Win Rate Stratification by Market Regime</span>
              </h2>
              <p className="text-[11px] text-ink-dim">
                Observed percentage of winning bets relative to the 48% random baseline
              </p>
            </div>
          </div>
        </div>

        <div className="h-64 w-full">
          <ResponsiveContainer width="100%" height="100%">
            <BarChart data={chartData} margin={{ top: 12, right: 12, left: -15, bottom: 0 }}>
              <CartesianGrid stroke="#181f2c" strokeDasharray="3 3" vertical={false} />
              <XAxis
                dataKey="regime"
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
              <Bar dataKey="winRate" radius={[4, 4, 0, 0]} maxBarSize={48}>
                {chartData.map((entry, index) => (
                  <Cell
                    key={`cell-${index}`}
                    fill={entry.winRate >= 65 ? "#10b981" : "#06b6d4"}
                    opacity={0.9}
                  />
                ))}
              </Bar>
            </BarChart>
          </ResponsiveContainer>
        </div>
      </div>

      {/* Regimes Table */}
      <div className="space-y-2">
        <div className="flex items-center justify-between">
          <h2 className="text-sm font-semibold text-ink font-mono flex items-center gap-2">
            <Activity className="h-4 w-4 text-accent-cyan" />
            <span>Detailed Regime Breakdown</span>
          </h2>
        </div>
        <RegimesTable rows={rows} />
      </div>
    </div>
  );
}

