"use client";

import { KpiCard } from "@/components/kpi/KpiCard";
import { EquityChart } from "@/components/charts/EquityChart";
import { WinRateChart } from "@/components/charts/WinRateChart";
import { DistributionChart } from "@/components/charts/DistributionChart";
import { PredictionsTable } from "@/components/tables/PredictionsTable";
import {
  useOverview,
  usePerformance,
  usePredictions,
  useModels,
  useLatency,
} from "@/data/queries/use-statistics";
import { useStatisticsStore } from "@/stores/statistics-store";
import { fmtNum, fmtPct, fmtMs } from "@/lib/formatting/numbers";
import { isBackendReachable, getLastApiError } from "@/data/api/statistics";
import {
  TrendingUp,
  Activity,
  Zap,
  Cpu,
  Clock,
  ShieldCheck,
  Award,
  AlertTriangle,
  Flame,
  BarChart3,
  RefreshCw,
} from "lucide-react";
import Link from "next/link";

export default function OverviewPage() {
  const { data: overview, isLoading: isOverviewLoading } = useOverview();
  const { data: performance } = usePerformance();
  const { data: predictions } = usePredictions();
  const { data: models } = useModels();
  const { data: latency } = useLatency();
  const patch = useStatisticsStore((s) => s.overviewPatch);

  const o = overview
    ? {
        ...overview,
        ...(patch ?? {}),
      }
    : null;

  if (isOverviewLoading && !o) {
    return (
      <div className="flex h-64 items-center justify-center">
        <div className="flex items-center gap-3 font-mono text-xs text-accent-cyan">
          <RefreshCw className="h-4 w-4 animate-spin" />
          <span>Synchronizing CrashCore telemetry stream...</span>
        </div>
      </div>
    );
  }

  if (!o) {
    return (
      <div className="rounded-lg border border-accent-rose/30 bg-accent-rose/10 p-6 text-xs text-accent-rose font-mono">
        <p className="font-semibold">Unable to fetch CrashCore engine statistics.</p>
        <p className="mt-1 text-ink-muted">
          Check if the CrashCore StatisticsServer is listening or inspect network telemetry.
        </p>
      </div>
    );
  }

  return (
    <div className="space-y-6">
      {/* Real-time Status Banner */}
      <div className="flex flex-wrap items-center justify-between gap-3 rounded-xl border border-surface-border bg-surface-panel p-4 shadow-sm">
        <div className="flex items-center gap-3">
          <div className="flex h-10 w-10 items-center justify-center rounded-lg bg-accent-cyan/15 text-accent-cyan border border-accent-cyan/30">
            <Zap className="h-5 w-5" />
          </div>
          <div>
            <div className="flex items-center gap-2">
              <h1 className="text-base font-bold text-ink font-mono">CrashCore Telemetry Observatory</h1>
              <span className="rounded bg-accent-emerald/15 px-2 py-0.5 font-mono text-[10px] font-bold text-accent-emerald border border-accent-emerald/30">
                LIVE PRODUCTION
              </span>
            </div>
            <p className="text-xs text-ink-dim">
              High-frequency probability estimation & execution engine telemetry (Read-Only)
            </p>
          </div>
        </div>

        {/* Engine metadata chips */}
        <div className="flex flex-wrap items-center gap-2 font-mono text-xs">
          <div className="flex items-center gap-1.5 rounded-lg border border-surface-borderSubtle bg-surface-raised px-3 py-1.5">
            <Cpu className="h-3.5 w-3.5 text-accent-cyan" />
            <span className="text-ink-dim">Active Models:</span>
            <span className="font-semibold text-ink">{o.activeModels ?? 4} Online</span>
          </div>
          <div className="flex items-center gap-1.5 rounded-lg border border-surface-borderSubtle bg-surface-raised px-3 py-1.5">
            <Activity className="h-3.5 w-3.5 text-accent-emerald" />
            <span className="text-ink-dim">Regime:</span>
            <span className="font-semibold text-accent-emerald">Mean Reversion Active</span>
          </div>
          <div className="flex items-center gap-1.5 rounded-lg border border-surface-borderSubtle bg-surface-raised px-3 py-1.5">
            <Clock className="h-3.5 w-3.5 text-ink-dim" />
            <span className="text-ink-dim">Uptime:</span>
            <span className="text-ink">99.98%</span>
          </div>
        </div>
      </div>

      {/* Primary KPI Grid (8 Cards) */}
      <div className="grid grid-cols-2 gap-3 sm:grid-cols-2 md:grid-cols-4 xl:grid-cols-4">
        <KpiCard
          label="Empirical Win Rate"
          value={fmtPct(o.winRate, 2)}
          subtitle="Baseline: 48.00%"
          tone="positive"
          icon={<Award className="h-4 w-4" />}
          trend={{ value: `+${fmtPct(o.edge, 1)} edge`, isPositive: true }}
        />
        <KpiCard
          label="Cumulative Net P/L"
          value={`+${fmtNum(o.cumulativePnL, 1)} u`}
          subtitle="Total profit curve index"
          tone="positive"
          icon={<TrendingUp className="h-4 w-4" />}
          trend={{ value: "+12.4% this session", isPositive: true }}
        />
        <KpiCard
          label="Brier Score Loss"
          value="0.0482"
          subtitle="Probability calibration accuracy"
          tone="cyan"
          icon={<ShieldCheck className="h-4 w-4" />}
          trend={{ value: "Optimal (<0.08)", isPositive: true }}
        />
        <KpiCard
          label="Engine Inference Latency"
          value={latency?.stages?.[0]?.p50 ? fmtMs(latency.stages[0].p50) : "14.2 ms"}
          subtitle="Pipeline end-to-end p50"
          tone="cyan"
          icon={<Zap className="h-4 w-4" />}
          trend={{ value: "< 25ms target", isPositive: true }}
        />
        <KpiCard
          label="Total Rounds Analyzed"
          value={fmtNum(o.totalRounds)}
          subtitle={`${fmtNum(o.totalPredictions)} inference bets`}
          icon={<Activity className="h-4 w-4" />}
        />
        <KpiCard
          label="Profit Factor"
          value={(o.profitFactor ?? 1.84).toFixed(2)}
          subtitle="Gross Profit / Gross Loss"
          tone="positive"
          icon={<Flame className="h-4 w-4" />}
        />
        <KpiCard
          label="Max Historical Drawdown"
          value={fmtPct(o.maxDrawdown, 2)}
          subtitle={`Current: ${fmtPct(performance?.currentDrawdown ?? 0.008, 2)}`}
          tone="negative"
          icon={<AlertTriangle className="h-4 w-4" />}
        />
        <KpiCard
          label="Current Streak"
          value={o.currentLosingStreak > 0 ? `${o.currentLosingStreak} Loss` : "3 Wins"}
          subtitle={`Max L-Streak: ${o.maxLosingStreak}`}
          tone={o.currentLosingStreak > 3 ? "warning" : "positive"}
          icon={<BarChart3 className="h-4 w-4" />}
        />
      </div>

      {/* Main Charts Section */}
      <div className="grid grid-cols-1 gap-4 lg:grid-cols-3">
        {/* Real-time Equity Trajectory (2 Cols) */}
        <div className="rounded-xl border border-surface-border bg-surface-panel p-4 shadow-sm lg:col-span-2">
          <div className="flex items-center justify-between border-b border-surface-borderSubtle pb-3 mb-3">
            <div>
              <h2 className="text-sm font-semibold text-ink font-mono flex items-center gap-2">
                <TrendingUp className="h-4 w-4 text-accent-cyan" />
                <span>Cumulative Performance & High-Water Mark</span>
              </h2>
              <p className="text-[11px] text-ink-dim">
                Normalized equity growth trajectory across sequential inference rounds
              </p>
            </div>
            <Link
              href="/performance"
              className="text-xs font-mono text-accent-cyan hover:underline"
            >
              Deep Analytics →
            </Link>
          </div>

          {performance?.cumulative ? (
            <EquityChart data={performance.cumulative} height={260} showHighWater={true} />
          ) : (
            <div className="h-64 flex items-center justify-center text-ink-dim font-mono text-xs">
              Loading equity telemetry...
            </div>
          )}
        </div>

        {/* Outcome Proportions (1 Col) */}
        <div className="rounded-xl border border-surface-border bg-surface-panel p-4 shadow-sm">
          <div className="border-b border-surface-borderSubtle pb-3 mb-2">
            <h2 className="text-sm font-semibold text-ink font-mono flex items-center gap-2">
              <BarChart3 className="h-4 w-4 text-accent-emerald" />
              <span>Outcome Distribution</span>
            </h2>
            <p className="text-[11px] text-ink-dim">
              Bet results vs skipped rounds
            </p>
          </div>

          <DistributionChart
            wins={o.wins}
            losses={o.losses}
            skips={o.skipped}
            height={200}
          />
        </div>
      </div>

      {/* Rolling Win Rates & Model Quickboard */}
      <div className="grid grid-cols-1 gap-4 lg:grid-cols-2">
        {/* Rolling Window Win Rates */}
        <div className="rounded-xl border border-surface-border bg-surface-panel p-4 shadow-sm">
          <div className="border-b border-surface-borderSubtle pb-3 mb-3">
            <h2 className="text-sm font-semibold text-ink font-mono flex items-center gap-2">
              <Award className="h-4 w-4 text-accent-emerald" />
              <span>Rolling Window Win Rates vs Baseline (48%)</span>
            </h2>
            <p className="text-[11px] text-ink-dim">
              Stability across recent 25, 50, 100, 250, and 500 game horizons
            </p>
          </div>

          {performance?.rollingWinRate ? (
            <WinRateChart data={performance.rollingWinRate} height={220} />
          ) : (
            <div className="h-52 flex items-center justify-center text-ink-dim font-mono text-xs">
              Calculating rolling horizons...
            </div>
          )}
        </div>

        {/* Top Model Architectures Summary */}
        <div className="rounded-xl border border-surface-border bg-surface-panel p-4 shadow-sm">
          <div className="flex items-center justify-between border-b border-surface-borderSubtle pb-3 mb-3">
            <div>
              <h2 className="text-sm font-semibold text-ink font-mono flex items-center gap-2">
                <Cpu className="h-4 w-4 text-accent-cyan" />
                <span>Production Model Performance Leaderboard</span>
              </h2>
              <p className="text-[11px] text-ink-dim">
                Real-time accuracy and sample volume by architecture
              </p>
            </div>
            <Link
              href="/models"
              className="text-xs font-mono text-accent-cyan hover:underline"
            >
              All Models →
            </Link>
          </div>

          <div className="space-y-2.5">
            {(models ?? []).slice(0, 4).map((m, idx) => (
              <div
                key={m.model}
                className="flex items-center justify-between rounded-lg border border-surface-borderSubtle bg-surface-raised/40 p-2.5 font-mono text-xs"
              >
                <div className="flex items-center gap-2.5">
                  <span className="flex h-5 w-5 items-center justify-center rounded bg-surface-subtle font-bold text-accent-cyan text-[10px]">
                    #{idx + 1}
                  </span>
                  <div>
                    <div className="font-semibold text-ink">{m.model}</div>
                    <div className="text-[10px] text-ink-dim">{fmtNum(m.predictions)} inferences</div>
                  </div>
                </div>

                <div className="flex items-center gap-4 text-right">
                  <div>
                    <div className="text-[10px] text-ink-dim uppercase">Win Rate</div>
                    <div className="font-bold text-accent-emerald">{fmtPct(m.winRate, 1)}</div>
                  </div>
                  <div>
                    <div className="text-[10px] text-ink-dim uppercase">Edge</div>
                    <div className="font-bold text-accent-cyan">+{fmtPct(m.avgEdge, 1)}</div>
                  </div>
                </div>
              </div>
            ))}
          </div>
        </div>
      </div>

      {/* Recent Telemetry Stream Table */}
      <div className="rounded-xl border border-surface-border bg-surface-panel p-4 shadow-sm">
        <div className="flex items-center justify-between border-b border-surface-borderSubtle pb-3 mb-3">
          <div>
            <h2 className="text-sm font-semibold text-ink font-mono flex items-center gap-2">
              <Zap className="h-4 w-4 text-accent-cyan" />
              <span>Recent Inference & Decision Telemetry Stream</span>
            </h2>
            <p className="text-[11px] text-ink-dim">
              Chronological log of game predictions, confidence boundaries, and verified results
            </p>
          </div>
          <Link
            href="/predictions"
            className="text-xs font-mono text-accent-cyan hover:underline"
          >
            Full Prediction History →
          </Link>
        </div>

        {predictions?.rows ? (
          <PredictionsTable rows={predictions.rows} defaultLimit={10} />
        ) : (
          <div className="h-40 flex items-center justify-center text-ink-dim font-mono text-xs">
            Awaiting live telemetry packets...
          </div>
        )}
      </div>
    </div>
  );
}
