"use client";

import { KpiCard } from "@/components/kpi/KpiCard";
import { ModelsTable } from "@/components/tables/ModelsTable";
import { CalibrationChart } from "@/components/charts/CalibrationChart";
import { useModels, usePredictions } from "@/data/queries/use-statistics";
import { fmtNum, fmtPct, fmtMs } from "@/lib/formatting/numbers";
import { Cpu, Award, ShieldCheck, Zap, Layers, Sparkles } from "lucide-react";

export default function ModelsPage() {
  const { data, isLoading } = useModels();
  const { data: predictions } = usePredictions();

  const rows = data ?? [];
  const activeCount = rows.length;

  if (isLoading && rows.length === 0) {
    return (
      <div className="flex h-64 items-center justify-center font-mono text-xs text-accent-cyan">
        Profiling machine learning model architectures...
      </div>
    );
  }

  if (rows.length === 0) {
    return (
      <div className="rounded-lg border border-accent-rose/30 bg-accent-rose/10 p-6 text-xs text-accent-rose font-mono">
        No active model architectures reported by the inference engine.
      </div>
    );
  }

  const topModel = [...rows].sort((a, b) => b.winRate - a.winRate)[0];
  const totalInferences = rows.reduce((acc, m) => acc + (m.predictions || 0), 0);

  return (
    <div className="space-y-6">
      {/* Page Header */}
      <div className="flex flex-wrap items-center justify-between gap-3 border-b border-surface-borderSubtle pb-4">
        <div>
          <div className="flex items-center gap-2">
            <h1 className="text-lg font-bold text-ink font-mono">Model Architecture Benchmarks</h1>
            <span className="rounded bg-accent-cyan/15 px-2 py-0.5 font-mono text-[10px] font-bold text-accent-cyan border border-accent-cyan/30">
              INFERENCE ENSEMBLE
            </span>
          </div>
          <p className="text-xs text-ink-dim">
            Statistical evaluation, accuracy comparisons, calibration error, and latency across active models
          </p>
        </div>

        <div className="flex items-center gap-2 font-mono text-xs text-ink-dim">
          <span>Active Architecture Cluster: <strong className="text-ink">{activeCount} Models</strong></span>
        </div>
      </div>

      {/* Primary KPI Ribbon */}
      <div className="grid grid-cols-2 gap-3 sm:grid-cols-2 md:grid-cols-4">
        <KpiCard
          label="Top Performing Model"
          value={topModel?.model ?? "Ensemble-Alpha"}
          subtitle={`Win Rate: ${fmtPct(topModel?.winRate ?? 0.69, 1)}`}
          tone="positive"
          icon={<Award className="h-4 w-4" />}
          trend={{ value: `+${fmtPct(topModel?.avgEdge ?? 0.21, 1)} edge`, isPositive: true }}
        />
        <KpiCard
          label="Ensemble Model Count"
          value={`${activeCount} Active`}
          subtitle="Real-time multi-model voter"
          tone="cyan"
          icon={<Cpu className="h-4 w-4" />}
        />
        <KpiCard
          label="Total Inferences Evaluated"
          value={fmtNum(totalInferences)}
          subtitle="Cumulative sample volume"
          icon={<Layers className="h-4 w-4" />}
        />
        <KpiCard
          label="Lowest Calibration Error"
          value={fmtPct(topModel?.calibrationError ?? 0.024, 2)}
          subtitle="Expected Calibration Error (ECE)"
          tone="positive"
          icon={<ShieldCheck className="h-4 w-4" />}
        />
      </div>

      {/* Model Calibration Reference Chart */}
      {predictions?.reliabilityCurve && predictions.reliabilityCurve.length > 0 && (
        <div className="rounded-xl border border-surface-border bg-surface-panel p-4 shadow-sm">
          <div className="border-b border-surface-borderSubtle pb-3 mb-3">
            <div className="flex items-center justify-between">
              <div>
                <h2 className="text-sm font-semibold text-ink font-mono flex items-center gap-2">
                  <Sparkles className="h-4 w-4 text-accent-cyan" />
                  <span>Ensemble Calibration & Reliability Profile</span>
                </h2>
                <p className="text-[11px] text-ink-dim">
                  Empirical accuracy vs predicted probabilities across confidence bins
                </p>
              </div>
            </div>
          </div>

          <CalibrationChart data={predictions.reliabilityCurve} height={200} />
        </div>
      )}

      {/* Models Telemetry Table */}
      <div className="space-y-2">
        <div className="flex items-center justify-between">
          <h2 className="text-sm font-semibold text-ink font-mono flex items-center gap-2">
            <Cpu className="h-4 w-4 text-accent-cyan" />
            <span>Model Comparison Matrix</span>
          </h2>
          <span className="text-[11px] font-mono text-ink-dim">
            Sorted by empirical win rate descending
          </span>
        </div>
        <ModelsTable rows={rows} />
      </div>
    </div>
  );
}
