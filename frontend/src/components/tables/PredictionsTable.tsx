"use client";

import { useState, useMemo } from "react";
import type { PredictionRow } from "@/types/statistics";
import { fmtPct, fmtMs, fmtMult, fmtDateTime } from "@/lib/formatting/numbers";
import clsx from "clsx";
import {
  Search,
  Filter,
  ArrowUpDown,
  ChevronLeft,
  ChevronRight,
  Info,
  X,
  CheckCircle2,
  XCircle,
  Clock,
  Sparkles,
  Zap,
} from "lucide-react";

type SortField =
  | "gameId"
  | "timestamp"
  | "target"
  | "probability"
  | "confidence"
  | "edge"
  | "latencyMs";

type SortOrder = "asc" | "desc";

export function PredictionsTable({
  rows,
  defaultLimit = 15,
}: {
  rows: PredictionRow[];
  defaultLimit?: number;
}) {
  const [searchTerm, setSearchTerm] = useState("");
  const [modelFilter, setModelFilter] = useState("ALL");
  const [regimeFilter, setRegimeFilter] = useState("ALL");
  const [decisionFilter, setDecisionFilter] = useState("ALL");
  const [resultFilter, setResultFilter] = useState("ALL");
  const [sortField, setSortField] = useState<SortField>("timestamp");
  const [sortOrder, setSortOrder] = useState<SortOrder>("desc");
  const [pageSize, setPageSize] = useState(defaultLimit);
  const [currentPage, setCurrentPage] = useState(1);
  const [selectedRow, setSelectedRow] = useState<PredictionRow | null>(null);

  // Extract unique models & regimes
  const availableModels = useMemo(() => {
    return Array.from(new Set(rows.map((r) => r.model))).sort();
  }, [rows]);

  const availableRegimes = useMemo(() => {
    return Array.from(new Set(rows.map((r) => r.regime))).sort();
  }, [rows]);

  // Filtered and sorted data
  const filteredRows = useMemo(() => {
    return rows.filter((r) => {
      if (searchTerm) {
        const term = searchTerm.toLowerCase();
        const matchesId = r.gameId.toLowerCase().includes(term);
        const matchesModel = r.model.toLowerCase().includes(term);
        if (!matchesId && !matchesModel) return false;
      }
      if (modelFilter !== "ALL" && r.model !== modelFilter) return false;
      if (regimeFilter !== "ALL" && r.regime !== regimeFilter) return false;
      if (decisionFilter !== "ALL" && r.decision !== decisionFilter) return false;
      if (resultFilter !== "ALL" && r.actualResult !== resultFilter) return false;
      return true;
    });
  }, [rows, searchTerm, modelFilter, regimeFilter, decisionFilter, resultFilter]);

  const sortedRows = useMemo(() => {
    return [...filteredRows].sort((a, b) => {
      let comparison = 0;
      if (sortField === "timestamp") {
        comparison = new Date(a.timestamp).getTime() - new Date(b.timestamp).getTime();
      } else if (sortField === "gameId") {
        comparison = a.gameId.localeCompare(b.gameId, undefined, { numeric: true });
      } else {
        comparison = (a[sortField] ?? 0) - (b[sortField] ?? 0);
      }
      return sortOrder === "asc" ? comparison : -comparison;
    });
  }, [filteredRows, sortField, sortOrder]);

  // Pagination
  const totalPages = Math.max(1, Math.ceil(sortedRows.length / pageSize));
  const paginatedRows = useMemo(() => {
    const start = (currentPage - 1) * pageSize;
    return sortedRows.slice(start, start + pageSize);
  }, [sortedRows, currentPage, pageSize]);

  const toggleSort = (field: SortField) => {
    if (sortField === field) {
      setSortOrder(sortOrder === "asc" ? "desc" : "asc");
    } else {
      setSortField(field);
      setSortOrder("desc");
    }
    setCurrentPage(1);
  };

  return (
    <div className="space-y-3">
      {/* Search & Filter Toolbar */}
      <div className="flex flex-wrap items-center justify-between gap-2.5 rounded-lg border border-surface-border bg-surface-panel p-3">
        <div className="flex flex-wrap items-center gap-2">
          {/* Search box */}
          <div className="relative">
            <Search className="absolute left-2.5 top-1/2 h-3.5 w-3.5 -translate-y-1/2 text-ink-dim" />
            <input
              type="text"
              placeholder="Search Game ID..."
              value={searchTerm}
              onChange={(e) => {
                setSearchTerm(e.target.value);
                setCurrentPage(1);
              }}
              className="w-44 sm:w-52 rounded-md border border-surface-border bg-surface-raised pl-8 pr-2.5 py-1 text-xs text-ink placeholder-ink-dim outline-none focus:border-accent-cyan/60"
            />
            {searchTerm && (
              <button
                onClick={() => setSearchTerm("")}
                className="absolute right-2 top-1/2 -translate-y-1/2 text-ink-dim hover:text-ink"
              >
                <X className="h-3 w-3" />
              </button>
            )}
          </div>

          {/* Model Filter */}
          <select
            value={modelFilter}
            onChange={(e) => {
              setModelFilter(e.target.value);
              setCurrentPage(1);
            }}
            className="rounded-md border border-surface-border bg-surface-raised px-2.5 py-1 text-xs text-ink outline-none hover:border-surface-subtle cursor-pointer"
          >
            <option value="ALL">All Models ({availableModels.length})</option>
            {availableModels.map((m) => (
              <option key={m} value={m}>
                {m}
              </option>
            ))}
          </select>

          {/* Regime Filter */}
          <select
            value={regimeFilter}
            onChange={(e) => {
              setRegimeFilter(e.target.value);
              setCurrentPage(1);
            }}
            className="rounded-md border border-surface-border bg-surface-raised px-2.5 py-1 text-xs text-ink outline-none hover:border-surface-subtle cursor-pointer"
          >
            <option value="ALL">All Regimes ({availableRegimes.length})</option>
            {availableRegimes.map((r) => (
              <option key={r} value={r}>
                {r}
              </option>
            ))}
          </select>

          {/* Decision Filter */}
          <select
            value={decisionFilter}
            onChange={(e) => {
              setDecisionFilter(e.target.value);
              setCurrentPage(1);
            }}
            className="rounded-md border border-surface-border bg-surface-raised px-2.5 py-1 text-xs text-ink outline-none hover:border-surface-subtle cursor-pointer"
          >
            <option value="ALL">All Decisions</option>
            <option value="Bet">Bet</option>
            <option value="Skip">Skip</option>
          </select>

          {/* Result Filter */}
          <select
            value={resultFilter}
            onChange={(e) => {
              setResultFilter(e.target.value);
              setCurrentPage(1);
            }}
            className="rounded-md border border-surface-border bg-surface-raised px-2.5 py-1 text-xs text-ink outline-none hover:border-surface-subtle cursor-pointer"
          >
            <option value="ALL">All Outcomes</option>
            <option value="Win">Win</option>
            <option value="Loss">Loss</option>
          </select>
        </div>

        {/* Results counter & active filters reset */}
        <div className="flex items-center gap-2 text-xs text-ink-dim font-mono">
          <span>
            Showing <strong className="text-ink">{sortedRows.length}</strong> of{" "}
            {rows.length} rows
          </span>
          {(searchTerm ||
            modelFilter !== "ALL" ||
            regimeFilter !== "ALL" ||
            decisionFilter !== "ALL" ||
            resultFilter !== "ALL") && (
            <button
              onClick={() => {
                setSearchTerm("");
                setModelFilter("ALL");
                setRegimeFilter("ALL");
                setDecisionFilter("ALL");
                setResultFilter("ALL");
                setCurrentPage(1);
              }}
              className="rounded bg-surface-subtle px-1.5 py-0.5 text-[10px] text-accent-cyan hover:bg-surface-raised"
            >
              Reset
            </button>
          )}
        </div>
      </div>

      {/* Table Container */}
      <div className="overflow-x-auto rounded-lg border border-surface-border bg-surface-panel shadow-sm">
        <table className="w-full min-w-[960px] text-left text-xs">
          <thead className="border-b border-surface-border bg-surface-raised/80 font-mono text-[11px] text-ink-dim uppercase tracking-wider">
            <tr>
              <th
                onClick={() => toggleSort("gameId")}
                className="cursor-pointer px-3 py-2.5 font-medium hover:text-ink transition-colors"
              >
                <div className="flex items-center gap-1">
                  <span>Game ID</span>
                  <ArrowUpDown className="h-3 w-3 opacity-60" />
                </div>
              </th>
              <th
                onClick={() => toggleSort("timestamp")}
                className="cursor-pointer px-3 py-2.5 font-medium hover:text-ink transition-colors"
              >
                <div className="flex items-center gap-1">
                  <span>Timestamp</span>
                  <ArrowUpDown className="h-3 w-3 opacity-60" />
                </div>
              </th>
              <th
                onClick={() => toggleSort("target")}
                className="cursor-pointer px-3 py-2.5 font-medium hover:text-ink transition-colors"
              >
                <div className="flex items-center gap-1">
                  <span>Target</span>
                  <ArrowUpDown className="h-3 w-3 opacity-60" />
                </div>
              </th>
              <th
                onClick={() => toggleSort("probability")}
                className="cursor-pointer px-3 py-2.5 font-medium hover:text-ink transition-colors"
              >
                <div className="flex items-center gap-1">
                  <span>Prob / Conf</span>
                  <ArrowUpDown className="h-3 w-3 opacity-60" />
                </div>
              </th>
              <th
                onClick={() => toggleSort("edge")}
                className="cursor-pointer px-3 py-2.5 font-medium hover:text-ink transition-colors"
              >
                <div className="flex items-center gap-1">
                  <span>Edge</span>
                  <ArrowUpDown className="h-3 w-3 opacity-60" />
                </div>
              </th>
              <th className="px-3 py-2.5 font-medium">Model & Regime</th>
              <th className="px-3 py-2.5 font-medium">Decision</th>
              <th className="px-3 py-2.5 font-medium">Outcome</th>
              <th
                onClick={() => toggleSort("latencyMs")}
                className="cursor-pointer px-3 py-2.5 font-medium hover:text-ink transition-colors text-right"
              >
                <div className="flex items-center justify-end gap-1">
                  <span>Inference</span>
                  <ArrowUpDown className="h-3 w-3 opacity-60" />
                </div>
              </th>
            </tr>
          </thead>
          <tbody className="divide-y divide-surface-borderSubtle">
            {paginatedRows.length === 0 ? (
              <tr>
                <td colSpan={9} className="py-8 text-center text-ink-dim font-mono text-xs">
                  No prediction telemetry records match the selected filter criteria.
                </td>
              </tr>
            ) : (
              paginatedRows.map((r) => {
                const isWin = r.actualResult === "Win";
                const isLoss = r.actualResult === "Loss";
                const isBet = r.decision === "Bet";

                return (
                  <tr
                    key={r.gameId}
                    onClick={() => setSelectedRow(r)}
                    className="cursor-pointer transition-colors hover:bg-surface-raised/50"
                  >
                    {/* Game ID */}
                    <td className="px-3 py-2 font-mono text-ink font-medium tabular-nums">
                      <div className="flex items-center gap-1.5">
                        <span className="text-accent-cyan/80">#</span>
                        <span>{r.gameId}</span>
                      </div>
                    </td>

                    {/* Timestamp */}
                    <td className="px-3 py-2 font-mono text-ink-muted text-[11px] tabular-nums whitespace-nowrap">
                      {fmtDateTime(r.timestamp)}
                    </td>

                    {/* Target Multiplier */}
                    <td className="px-3 py-2 font-mono text-ink font-semibold tabular-nums">
                      {fmtMult(r.target)}
                    </td>

                    {/* Probability & Confidence Bar */}
                    <td className="px-3 py-2 font-mono tabular-nums">
                      <div className="w-28 space-y-1">
                        <div className="flex items-center justify-between text-[11px]">
                          <span className="text-ink font-semibold">{fmtPct(r.probability, 1)}</span>
                          <span className="text-[10px] text-ink-dim">{fmtPct(r.confidence, 0)}c</span>
                        </div>
                        <div className="h-1.5 w-full rounded-full bg-surface-subtle overflow-hidden">
                          <div
                            className={clsx(
                              "h-full rounded-full",
                              r.probability >= 0.8
                                ? "bg-accent-emerald"
                                : r.probability >= 0.65
                                ? "bg-accent-cyan"
                                : "bg-accent-amber"
                            )}
                            style={{ width: `${Math.min(100, r.probability * 100)}%` }}
                          />
                        </div>
                      </div>
                    </td>

                    {/* Edge vs Baseline */}
                    <td className="px-3 py-2 font-mono tabular-nums">
                      <span
                        className={clsx(
                          "font-semibold",
                          r.edge > 0.15
                            ? "text-accent-emerald"
                            : r.edge > 0
                            ? "text-accent-cyan"
                            : "text-accent-rose"
                        )}
                      >
                        +{fmtPct(r.edge, 1)}
                      </span>
                    </td>

                    {/* Model & Regime */}
                    <td className="px-3 py-2">
                      <div className="text-ink font-medium leading-tight">{r.model}</div>
                      <div className="text-[10px] text-ink-dim font-mono">{r.regime}</div>
                    </td>

                    {/* Decision */}
                    <td className="px-3 py-2 font-mono">
                      <span
                        className={clsx(
                          "inline-flex items-center gap-1 rounded px-2 py-0.5 text-[10px] font-semibold uppercase tracking-wide",
                          isBet
                            ? "bg-accent-cyan/15 text-accent-cyan border border-accent-cyan/30"
                            : "bg-surface-subtle text-ink-dim border border-surface-border"
                        )}
                      >
                        {r.decision}
                      </span>
                    </td>

                    {/* Outcome Result */}
                    <td className="px-3 py-2 font-mono">
                      <span
                        className={clsx(
                          "inline-flex items-center gap-1 rounded px-2 py-0.5 text-[10px] font-semibold uppercase tracking-wider",
                          isWin && "bg-accent-emerald/15 text-accent-emerald border border-accent-emerald/30",
                          isLoss && "bg-accent-rose/15 text-accent-rose border border-accent-rose/30",
                          !isWin && !isLoss && "bg-surface-subtle text-ink-dim border border-surface-border"
                        )}
                      >
                        {isWin && <CheckCircle2 className="h-3 w-3" />}
                        {isLoss && <XCircle className="h-3 w-3" />}
                        <span>{r.actualResult}</span>
                      </span>
                    </td>

                    {/* Latency */}
                    <td className="px-3 py-2 text-right font-mono text-ink-muted tabular-nums">
                      <span
                        className={clsx(
                          r.latencyMs < 15
                            ? "text-accent-emerald"
                            : r.latencyMs < 35
                            ? "text-ink"
                            : "text-accent-amber"
                        )}
                      >
                        {fmtMs(r.latencyMs)}
                      </span>
                    </td>
                  </tr>
                );
              })
            )}
          </tbody>
        </table>
      </div>

      {/* Pagination Controls */}
      <div className="flex flex-wrap items-center justify-between gap-3 text-xs text-ink-dim font-mono pt-1">
        <div className="flex items-center gap-2">
          <span>Rows per page:</span>
          <select
            value={pageSize}
            onChange={(e) => {
              setPageSize(Number(e.target.value));
              setCurrentPage(1);
            }}
            className="rounded border border-surface-border bg-surface-raised px-2 py-1 text-xs text-ink outline-none cursor-pointer"
          >
            <option value={10}>10</option>
            <option value={15}>15</option>
            <option value={25}>25</option>
            <option value={50}>50</option>
          </select>
        </div>

        <div className="flex items-center gap-3">
          <span>
            Page <strong className="text-ink">{currentPage}</strong> of{" "}
            <strong className="text-ink">{totalPages}</strong>
          </span>
          <div className="flex items-center gap-1">
            <button
              onClick={() => setCurrentPage((p) => Math.max(1, p - 1))}
              disabled={currentPage === 1}
              className="flex h-7 w-7 items-center justify-center rounded border border-surface-border bg-surface-panel text-ink disabled:opacity-30 disabled:cursor-not-allowed hover:bg-surface-raised"
            >
              <ChevronLeft className="h-3.5 w-3.5" />
            </button>
            <button
              onClick={() => setCurrentPage((p) => Math.min(totalPages, p + 1))}
              disabled={currentPage === totalPages}
              className="flex h-7 w-7 items-center justify-center rounded border border-surface-border bg-surface-panel text-ink disabled:opacity-30 disabled:cursor-not-allowed hover:bg-surface-raised"
            >
              <ChevronRight className="h-3.5 w-3.5" />
            </button>
          </div>
        </div>
      </div>

      {/* Detailed Prediction Inspection Modal */}
      {selectedRow && (
        <div className="fixed inset-0 z-50 flex items-center justify-center bg-black/75 backdrop-blur-sm p-4">
          <div className="relative w-full max-w-lg rounded-xl border border-surface-border bg-surface-panel p-6 shadow-2xl">
            <div className="flex items-center justify-between border-b border-surface-border pb-3">
              <div className="flex items-center gap-2">
                <Sparkles className="h-4 w-4 text-accent-cyan" />
                <h3 className="text-sm font-semibold text-ink font-mono">
                  Game Telemetry #{selectedRow.gameId}
                </h3>
              </div>
              <button
                onClick={() => setSelectedRow(null)}
                className="rounded p-1 text-ink-dim hover:bg-surface-raised hover:text-ink"
              >
                <X className="h-4 w-4" />
              </button>
            </div>

            <div className="mt-4 space-y-3 font-mono text-xs">
              <div className="grid grid-cols-2 gap-2.5">
                <div className="rounded-lg border border-surface-borderSubtle bg-surface-raised/40 p-3">
                  <div className="text-[10px] text-ink-dim uppercase">Predicted Target</div>
                  <div className="text-lg font-bold text-accent-cyan tabular-nums mt-0.5">
                    {fmtMult(selectedRow.target)}
                  </div>
                </div>

                <div className="rounded-lg border border-surface-borderSubtle bg-surface-raised/40 p-3">
                  <div className="text-[10px] text-ink-dim uppercase">Actual Crash Multiplier</div>
                  <div className="text-lg font-bold text-ink tabular-nums mt-0.5">
                    {selectedRow.crashMultiplier != null
                      ? fmtMult(selectedRow.crashMultiplier)
                      : "—"}
                  </div>
                </div>
              </div>

              <div className="space-y-2 rounded-lg border border-surface-border bg-surface-raised/20 p-3.5">
                <div className="flex items-center justify-between">
                  <span className="text-ink-muted">Inference Model:</span>
                  <span className="text-ink font-bold">{selectedRow.model}</span>
                </div>
                <div className="flex items-center justify-between">
                  <span className="text-ink-muted">Market Regime:</span>
                  <span className="text-ink font-semibold">{selectedRow.regime}</span>
                </div>
                <div className="flex items-center justify-between">
                  <span className="text-ink-muted">Posterior Probability:</span>
                  <span className="text-accent-emerald font-semibold">
                    {fmtPct(selectedRow.probability, 4)}
                  </span>
                </div>
                <div className="flex items-center justify-between">
                  <span className="text-ink-muted">Confidence Bound:</span>
                  <span className="text-ink font-semibold">{fmtPct(selectedRow.confidence, 4)}</span>
                </div>
                <div className="flex items-center justify-between">
                  <span className="text-ink-muted">Empirical Edge vs Base:</span>
                  <span className="text-accent-cyan font-semibold">+{fmtPct(selectedRow.edge, 4)}</span>
                </div>
                {selectedRow.brierScore != null && (
                  <div className="flex items-center justify-between">
                    <span className="text-ink-muted">Individual Brier Loss:</span>
                    <span className="text-ink">{selectedRow.brierScore.toFixed(4)}</span>
                  </div>
                )}
                <div className="flex items-center justify-between">
                  <span className="text-ink-muted">Inference Latency:</span>
                  <span className="text-accent-emerald">{fmtMs(selectedRow.latencyMs)}</span>
                </div>
                <div className="flex items-center justify-between border-t border-surface-borderSubtle pt-2">
                  <span className="text-ink-muted">Recorded Timestamp:</span>
                  <span className="text-ink-dim text-[11px]">
                    {fmtDateTime(selectedRow.timestamp)}
                  </span>
                </div>
              </div>
            </div>

            <div className="mt-5 flex justify-end">
              <button
                onClick={() => setSelectedRow(null)}
                className="rounded border border-surface-border bg-surface-raised px-4 py-1.5 text-xs font-medium text-ink transition-colors hover:bg-surface-subtle"
              >
                Close Inspector
              </button>
            </div>
          </div>
        </div>
      )}
    </div>
  );
}
