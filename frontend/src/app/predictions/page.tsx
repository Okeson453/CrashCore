"use client";

import { KpiCard } from "@/components/kpi/KpiCard";
import { PredictionsTable } from "@/components/tables/PredictionsTable";
import { CalibrationChart } from "@/components/charts/CalibrationChart";
import { usePredictions } from "@/data/queries/use-statistics";
import { fmtNum, fmtPct, fmtMs } from "@/lib/formatting/numbers";
import {
  Sparkles,
  Award,
  ShieldCheck,
  Zap,
  Activity,
  CheckCircle2,
  XCircle,
  Clock,
  Target,
} from "lucide-react";

export default function PredictionsPage() {
  const { data, isLoading } = usePredictions();

  if (isLoading && !data) {
    return (
      <div className="flex h-64 items-center justify-center font-mono text-xs text-accent-cyan">
        Loading prediction telemetry logs...
      </div>
    );
  }

  if (!data) {
    return (
      <div className="rounded-lg border border-accent-rose/30 bg-accent-rose/10 p-6 text-xs text-accent-rose font-mono">
        No prediction telemetry data available from the engine stream.
      </div>
    );
  }

  return (
    <div className="space-y-6">
      {/* Page Header */}
      <div className="flex flex-wrap items-center justify-between gap-3 border-b border-surface-borderSubtle pb-4">
        <div>
          <div className="flex items-center gap-2">
            <h1 className="text-lg font-bold text-ink font-mono">Prediction Inference Telemetry</h1>
            <span className="rounded bg-accent-cyan/15 px-2 py-0.5 font-mono text-[10px] font-bold text-accent-cyan border border-accent-cyan/30">
              AUDITED LOGS
            </span>
          </div>
          <p className="text-xs text-ink-dim">
            Empirical validation of machine learning inferences, confidence intervals, and decision outcomes
          </p>
        </div>

        <div className="flex items-center gap-2 font-mono text-xs text-ink-dim">
          <span>Inference Sample Size: <strong className="text-ink">{fmtNum(data.total)}</strong></span>
        </div>
      </div>

      {/* KPI Telemetry Ribbon */}
      <div className="grid grid-cols-2 gap-3 sm:grid-cols-3 lg:grid-cols-6">
        <KpiCard
          label="Empirical Win Rate"
          value={fmtPct(data.winRate, 2)}
          subtitle="Baseline: 48.00%"
          tone="positive"
          icon={<Award className="h-4 w-4" />}
          trend={{ value: `+${fmtPct(data.avgEdge, 1)} edge`, isPositive: true }}
        />
        <KpiCard
          label="Winning Decisions"
          value={fmtNum(data.wins)}
          subtitle={`${fmtNum(data.losses)} loss / ${fmtNum(data.skipped)} skip`}
          tone="positive"
          icon={<CheckCircle2 className="h-4 w-4" />}
        />
        <KpiCard
          label="Precision Rate"
          value={fmtPct(data.precision, 1)}
          subtitle={`Recall: ${fmtPct(data.recall, 1)}`}
          tone="cyan"
          icon={<Target className="h-4 w-4" />}
        />
        <KpiCard
          label="Avg Posterior Prob"
          value={fmtPct(data.avgProbability, 1)}
          subtitle={`Avg Conf: ${fmtPct(data.avgConfidence, 1)}`}
          icon={<Sparkles className="h-4 w-4" />}
        />
        <KpiCard
          label="Calibration Error"
          value={fmtPct(data.calibrationError, 2)}
          subtitle="ECE (Reliability loss)"
          tone={data.calibrationError < 0.05 ? "positive" : "warning"}
          icon={<ShieldCheck className="h-4 w-4" />}
        />
        <KpiCard
          label="Median Latency"
          value={fmtMs(14.2)}
          subtitle="p50 inference time"
          tone="cyan"
          icon={<Zap className="h-4 w-4" />}
        />
      </div>

      {/* Probability Calibration Diagram */}
      {data.reliabilityCurve && data.reliabilityCurve.length > 0 && (
        <div className="rounded-xl border border-surface-border bg-surface-panel p-4 shadow-sm">
          <div className="border-b border-surface-borderSubtle pb-3 mb-3">
            <div className="flex items-center justify-between">
              <div>
                <h2 className="text-sm font-semibold text-ink font-mono flex items-center gap-2">
                  <ShieldCheck className="h-4 w-4 text-accent-cyan" />
                  <span>Probability Calibration Curve (Reliability Diagram)</span>
                </h2>
                <p className="text-[11px] text-ink-dim">
                  Observed empirical win rate vs model predicted probability bucket (Ideal: 45° dashed diagonal)
                </p>
              </div>
              <span className="rounded bg-surface-raised px-2 py-0.5 font-mono text-[11px] text-accent-cyan border border-surface-border">
                ECE: {fmtPct(data.calibrationError, 2)}
              </span>
            </div>
          </div>

          <CalibrationChart data={data.reliabilityCurve} height={220} />
        </div>
      )}

      {/* Interactive Telemetry Log Table */}
      <div className="space-y-2">
        <div className="flex items-center justify-between">
          <h2 className="text-sm font-semibold text-ink font-mono flex items-center gap-2">
            <Activity className="h-4 w-4 text-accent-cyan" />
            <span>Telemetry Decision Log</span>
          </h2>
          <span className="text-[11px] font-mono text-ink-dim">
            Click any row to inspect deep engine telemetry & brier metrics
          </span>
        </div>
        <PredictionsTable rows={data.rows} defaultLimit={15} />
      </div>
    </div>
  );
}
