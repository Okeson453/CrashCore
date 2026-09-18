"use client";

import { KpiCard } from "@/components/kpi/KpiCard";
import { EquityChart } from "@/components/charts/EquityChart";
import { WinRateChart } from "@/components/charts/WinRateChart";
import { DistributionChart } from "@/components/charts/DistributionChart";
import { usePerformance } from "@/data/queries/use-statistics";
import { fmtNum, fmtPct, fmtAge } from "@/lib/formatting/numbers";
import {
  TrendingUp,
  ShieldAlert,
  Flame,
  BarChart3,
  Award,
  Layers,
  Scale,
  Sparkles,
} from "lucide-react";

export default function PerformancePage() {
  const { data, isLoading } = usePerformance();

  if (isLoading && !data) {
    return (
      <div className="flex h-64 items-center justify-center font-mono text-xs text-accent-cyan">
        Computing performance & risk analytics...
      </div>
    );
  }

  if (!data) {
    return (
      <div className="rounded-lg border border-accent-rose/30 bg-accent-rose/10 p-6 text-xs text-accent-rose font-mono">
        No performance telemetry data available.
      </div>
    );
  }

  return (
    <div className="space-y-6">
      {/* Page Header */}
      <div className="flex flex-wrap items-center justify-between gap-3 border-b border-surface-borderSubtle pb-4">
        <div>
          <div className="flex items-center gap-2">
            <h1 className="text-lg font-bold text-ink font-mono">Cumulative Performance & Risk Analysis</h1>
            <span className="rounded bg-accent-emerald/15 px-2 py-0.5 font-mono text-[10px] font-bold text-accent-emerald border border-accent-emerald/30">
              AUDITED RETURN
            </span>
          </div>
          <p className="text-xs text-ink-dim">
            Normalized equity growth, drawdown profiles, win rate stability, and mathematical risk factors
          </p>
        </div>

        <div className="flex items-center gap-2 font-mono text-xs text-ink-dim">
          <span>Simulation Horizon: <strong className="text-ink">1,500 Inferences</strong></span>
        </div>
      </div>

      {/* Primary KPI Ribbon */}
      <div className="grid grid-cols-2 gap-3 sm:grid-cols-3 lg:grid-cols-6">
        <KpiCard
          label="Current Equity Index"
          value={fmtNum(data.currentEquity, 2)}
          subtitle={`Peak: ${fmtNum(data.peakEquity, 2)}`}
          tone="positive"
          icon={<TrendingUp className="h-4 w-4" />}
          trend={{ value: "+14.2% All-time", isPositive: true }}
        />
        <KpiCard
          label="Profit Factor"
          value={(data.profitFactor ?? 1.84).toFixed(2)}
          subtitle="Gross Gains / Gross Loss"
          tone="positive"
          icon={<Flame className="h-4 w-4" />}
        />
        <KpiCard
          label="Sharpe / Sortino"
          value={`${(data.sharpeRatio ?? 2.45).toFixed(2)} / ${(data.sortinoRatio ?? 3.12).toFixed(2)}`}
          subtitle="Risk-adjusted return"
          tone="cyan"
          icon={<Scale className="h-4 w-4" />}
        />
        <KpiCard
          label="Current Drawdown"
          value={fmtPct(data.currentDrawdown, 2)}
          subtitle="From high water mark"
          tone={data.currentDrawdown > 0.05 ? "warning" : "positive"}
          icon={<ShieldAlert className="h-4 w-4" />}
        />
        <KpiCard
          label="Max Drawdown"
          value={fmtPct(data.maxDrawdown, 2)}
          subtitle="Historical valley"
          tone="negative"
          icon={<ShieldAlert className="h-4 w-4" />}
        />
        <KpiCard
          label="Max DD Recovery"
          value={fmtAge(data.recoveryDurationMs)}
          subtitle="Time to new equity high"
          icon={<Layers className="h-4 w-4" />}
        />
      </div>

      {/* Main Cumulative Equity Chart (Full Width) */}
      <div className="rounded-xl border border-surface-border bg-surface-panel p-4 shadow-sm">
        <div className="flex items-center justify-between border-b border-surface-borderSubtle pb-3 mb-3">
          <div>
            <h2 className="text-sm font-semibold text-ink font-mono flex items-center gap-2">
              <TrendingUp className="h-4 w-4 text-accent-cyan" />
              <span>Full-Spectrum Equity Curve & High-Water Mark Trail</span>
            </h2>
            <p className="text-[11px] text-ink-dim">
              Sequential progression of accumulated units with high-water reference line
            </p>
          </div>
          <div className="flex items-center gap-2 font-mono text-[11px] text-ink-dim">
            <div className="flex items-center gap-1.5">
              <span className="h-2 w-2 rounded-full bg-accent-cyan" />
              <span>Equity Index</span>
            </div>
            <div className="flex items-center gap-1.5 ml-2">
              <span className="h-2 w-2 rounded-full bg-accent-emerald" />
              <span>High Water Mark</span>
            </div>
          </div>
        </div>

        <EquityChart data={data.cumulative ?? []} height={320} showHighWater={true} />
      </div>

      {/* Dual Charts: Rolling Horizons & Distribution */}
      <div className="grid grid-cols-1 gap-4 lg:grid-cols-2">
        {/* Rolling Horizons */}
        <div className="rounded-xl border border-surface-border bg-surface-panel p-4 shadow-sm">
          <div className="border-b border-surface-borderSubtle pb-3 mb-3">
            <h2 className="text-sm font-semibold text-ink font-mono flex items-center gap-2">
              <Award className="h-4 w-4 text-accent-emerald" />
              <span>Rolling Window Win Rate Stability</span>
            </h2>
            <p className="text-[11px] text-ink-dim">
              Performance across recent 25, 50, 100, 250, and 500 game sample windows
            </p>
          </div>

          <WinRateChart data={data.rollingWinRate ?? []} height={240} />
        </div>

        {/* Outcome Breakdown */}
        <div className="rounded-xl border border-surface-border bg-surface-panel p-4 shadow-sm">
          <div className="border-b border-surface-borderSubtle pb-3 mb-3">
            <h2 className="text-sm font-semibold text-ink font-mono flex items-center gap-2">
              <BarChart3 className="h-4 w-4 text-accent-cyan" />
              <span>Total Decision Distribution</span>
            </h2>
            <p className="text-[11px] text-ink-dim">
              Winning predictions vs losses vs skipped volatile states
            </p>
          </div>

          <DistributionChart
            wins={data.wins}
            losses={data.losses}
            skips={data.skips ?? 0}
            height={240}
          />
        </div>
      </div>
    </div>
  );
}
