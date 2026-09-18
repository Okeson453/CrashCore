"use client";

import { KpiCard } from "@/components/kpi/KpiCard";
import { LatencyTable } from "@/components/tables/LatencyTable";
import { LatencyStageChart } from "@/components/charts/LatencyStageChart";
import { useLatency } from "@/data/queries/use-statistics";
import { fmtNum, fmtMs } from "@/lib/formatting/numbers";
import { Zap, Activity, AlertTriangle, Layers, Clock, Cpu } from "lucide-react";

export default function LatencyPage() {
  const { data, isLoading } = useLatency();

  if (isLoading && !data) {
    return (
      <div className="flex h-64 items-center justify-center font-mono text-xs text-accent-cyan">
        Profiling sub-millisecond execution pipeline timings...
      </div>
    );
  }

  if (!data) {
    return (
      <div className="rounded-lg border border-accent-rose/30 bg-accent-rose/10 p-6 text-xs text-accent-rose font-mono">
        No latency telemetry logs available from the C++ StatisticsServer.
      </div>
    );
  }

  return (
    <div className="space-y-6">
      {/* Page Header */}
      <div className="flex flex-wrap items-center justify-between gap-3 border-b border-surface-borderSubtle pb-4">
        <div>
          <div className="flex items-center gap-2">
            <h1 className="text-lg font-bold text-ink font-mono">Pipeline Latency & Queue Health</h1>
            <span className="rounded bg-accent-cyan/15 px-2 py-0.5 font-mono text-[10px] font-bold text-accent-cyan border border-accent-cyan/30">
              LOW-LATENCY RUNTIME
            </span>
          </div>
          <p className="text-xs text-ink-dim">
            Sub-millisecond stage execution telemetry, lock-free ring buffer metrics, and queue pressure
          </p>
        </div>

        <div className="flex items-center gap-2 font-mono text-xs text-ink-dim">
          <span>Engine Pipeline Stages: <strong className="text-ink">{data.stages.length} Stages</strong></span>
        </div>
      </div>

      {/* Primary KPI Ribbon */}
      <div className="grid grid-cols-2 gap-3 sm:grid-cols-3 lg:grid-cols-6">
        <KpiCard
          label="Inbound Event Throughput"
          value={`${fmtNum(data.eventsPerSec, 1)} /s`}
          subtitle="WebSocket parse rate"
          tone="positive"
          icon={<Activity className="h-4 w-4" />}
          trend={{ value: "Nominal stream", isPositive: true }}
        />
        <KpiCard
          label="Prediction Rate"
          value={`${fmtNum(data.predictionsPerSec, 1)} /s`}
          subtitle="Model inferences/sec"
          tone="cyan"
          icon={<Cpu className="h-4 w-4" />}
        />
        <KpiCard
          label="Lock-Free Queue Depth"
          value={fmtNum(data.queueDepth)}
          subtitle="Current in-flight packets"
          tone={data.queueDepth > 100 ? "warning" : "positive"}
          icon={<Layers className="h-4 w-4" />}
        />
        <KpiCard
          label="Processing Backlog"
          value={fmtNum(data.processingBacklog)}
          subtitle="Unconsumed ring items"
          icon={<Clock className="h-4 w-4" />}
        />
        <KpiCard
          label="Dropped Packets"
          value={fmtNum(data.droppedEvents)}
          subtitle="Buffer overflow drops"
          tone={data.droppedEvents > 0 ? "negative" : "positive"}
          icon={<AlertTriangle className="h-4 w-4" />}
        />
        <KpiCard
          label="Duplicate Frames"
          value={fmtNum(data.duplicateEvents)}
          subtitle="Network frame replays"
          icon={<Zap className="h-4 w-4" />}
        />
      </div>

      {/* Latency Percentile Stage Chart */}
      <div className="rounded-xl border border-surface-border bg-surface-panel p-4 shadow-sm">
        <div className="border-b border-surface-borderSubtle pb-3 mb-3">
          <div className="flex items-center justify-between">
            <div>
              <h2 className="text-sm font-semibold text-ink font-mono flex items-center gap-2">
                <Zap className="h-4 w-4 text-accent-cyan" />
                <span>Critical Path Stage Latency Breakdown (p50 / p95 / p99)</span>
              </h2>
              <p className="text-[11px] text-ink-dim">
                Latency profiling measured across the complete socket ingestion, feature extraction, inference, and outbox path
              </p>
            </div>
            <div className="flex items-center gap-3 font-mono text-[11px]">
              <span className="flex items-center gap-1.5 text-accent-cyan">
                <span className="h-2 w-2 rounded-full bg-accent-cyan" />
                p50 Median
              </span>
              <span className="flex items-center gap-1.5 text-amber-400">
                <span className="h-2 w-2 rounded-full bg-amber-400" />
                p95 Tail
              </span>
              <span className="flex items-center gap-1.5 text-accent-rose">
                <span className="h-2 w-2 rounded-full bg-accent-rose" />
                p99 Extreme
              </span>
            </div>
          </div>
        </div>

        <LatencyStageChart stages={data.stages} height={260} />
      </div>

      {/* Latency Stage Table */}
      <div className="space-y-2">
        <div className="flex items-center justify-between">
          <h2 className="text-sm font-semibold text-ink font-mono flex items-center gap-2">
            <Layers className="h-4 w-4 text-accent-cyan" />
            <span>Execution Stage Micro-Benchmarks</span>
          </h2>
          <span className="text-[11px] font-mono text-ink-dim">
            Engine hardware budget: &lt; 25.0 ms end-to-end
          </span>
        </div>
        <LatencyTable stages={data.stages} />
      </div>
    </div>
  );
}
