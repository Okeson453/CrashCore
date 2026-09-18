"use client";

import { useState, useMemo } from "react";
import type { RoundRow } from "@/types/statistics";
import { fmtMult, fmtDateTime } from "@/lib/formatting/numbers";
import clsx from "clsx";
import { Search, X, ChevronLeft, ChevronRight, Hash, Copy, Check } from "lucide-react";

export function RoundsTable({
  rows,
  defaultLimit = 15,
}: {
  rows: RoundRow[];
  defaultLimit?: number;
}) {
  const [searchTerm, setSearchTerm] = useState("");
  const [pageSize, setPageSize] = useState(defaultLimit);
  const [currentPage, setCurrentPage] = useState(1);
  const [copiedHash, setCopiedHash] = useState<string | null>(null);

  const filteredRows = useMemo(() => {
    if (!searchTerm) return rows;
    const term = searchTerm.toLowerCase();
    return rows.filter(
      (r) =>
        r.gameId.toLowerCase().includes(term) ||
        (r.hash && r.hash.toLowerCase().includes(term)) ||
        (r.regime && r.regime.toLowerCase().includes(term))
    );
  }, [rows, searchTerm]);

  const totalPages = Math.max(1, Math.ceil(filteredRows.length / pageSize));
  const paginatedRows = useMemo(() => {
    const start = (currentPage - 1) * pageSize;
    return filteredRows.slice(start, start + pageSize);
  }, [filteredRows, currentPage, pageSize]);

  const copyToClipboard = (text: string) => {
    if (typeof navigator !== "undefined" && navigator.clipboard) {
      navigator.clipboard.writeText(text);
      setCopiedHash(text);
      setTimeout(() => setCopiedHash(null), 1800);
    }
  };

  return (
    <div className="space-y-3">
      {/* Search Toolbar */}
      <div className="flex flex-wrap items-center justify-between gap-2.5 rounded-lg border border-surface-border bg-surface-panel p-3">
        <div className="relative">
          <Search className="absolute left-2.5 top-1/2 h-3.5 w-3.5 -translate-y-1/2 text-ink-dim" />
          <input
            type="text"
            placeholder="Search Game ID, Hash, Regime..."
            value={searchTerm}
            onChange={(e) => {
              setSearchTerm(e.target.value);
              setCurrentPage(1);
            }}
            className="w-56 sm:w-72 rounded-md border border-surface-border bg-surface-raised pl-8 pr-2.5 py-1 text-xs text-ink placeholder-ink-dim outline-none focus:border-accent-cyan/60"
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

        <div className="text-xs text-ink-dim font-mono">
          Showing <strong className="text-ink">{filteredRows.length}</strong> recorded rounds
        </div>
      </div>

      {/* Table */}
      <div className="overflow-x-auto rounded-lg border border-surface-border bg-surface-panel shadow-sm">
        <table className="w-full min-w-[700px] text-left text-xs">
          <thead className="border-b border-surface-border bg-surface-raised/80 font-mono text-[11px] text-ink-dim uppercase tracking-wider">
            <tr>
              <th className="px-3 py-2.5 font-medium">Game ID</th>
              <th className="px-3 py-2.5 font-medium">Timestamp</th>
              <th className="px-3 py-2.5 font-medium">Crash Multiplier</th>
              <th className="px-3 py-2.5 font-medium">Market Regime</th>
              <th className="px-3 py-2.5 font-medium text-right">Provable Hash</th>
            </tr>
          </thead>
          <tbody className="divide-y divide-surface-borderSubtle">
            {paginatedRows.length === 0 ? (
              <tr>
                <td colSpan={5} className="py-8 text-center text-ink-dim font-mono text-xs">
                  No rounds match the search query.
                </td>
              </tr>
            ) : (
              paginatedRows.map((r) => {
                const multi = r.multiplier;
                const isMoon = multi >= 10.0;
                const isHigh = multi >= 3.0 && multi < 10.0;
                const isMed = multi >= 1.5 && multi < 3.0;
                const isBust = multi < 1.2;

                return (
                  <tr key={r.gameId} className="transition-colors hover:bg-surface-raised/50">
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

                    {/* Multiplier Badge */}
                    <td className="px-3 py-2 font-mono tabular-nums">
                      <span
                        className={clsx(
                          "inline-flex items-center rounded px-2 py-0.5 font-bold tabular-nums text-xs border",
                          isMoon && "bg-amber-400/15 text-amber-300 border-amber-400/30",
                          isHigh && "bg-accent-emerald/15 text-accent-emerald border-accent-emerald/30",
                          isMed && "bg-accent-cyan/15 text-accent-cyan border-accent-cyan/30",
                          isBust && "bg-accent-rose/15 text-accent-rose border-accent-rose/30",
                          !isMoon && !isHigh && !isMed && !isBust && "bg-surface-subtle text-ink border-surface-border"
                        )}
                      >
                        {fmtMult(multi)}
                      </span>
                    </td>

                    {/* Regime */}
                    <td className="px-3 py-2 text-ink-muted font-mono text-[11px]">
                      {r.regime ?? "—"}
                    </td>

                    {/* Provable Hash */}
                    <td className="px-3 py-2 text-right font-mono text-[11px] text-ink-dim">
                      {r.hash ? (
                        <button
                          onClick={() => copyToClipboard(r.hash!)}
                          title="Click to copy hash"
                          className="inline-flex items-center gap-1.5 hover:text-ink transition-colors font-mono"
                        >
                          <span className="truncate max-w-[140px] sm:max-w-[200px]">
                            {r.hash}
                          </span>
                          {copiedHash === r.hash ? (
                            <Check className="h-3 w-3 text-accent-emerald shrink-0" />
                          ) : (
                            <Copy className="h-3 w-3 opacity-60 hover:opacity-100 shrink-0" />
                          )}
                        </button>
                      ) : (
                        "—"
                      )}
                    </td>
                  </tr>
                );
              })
            )}
          </tbody>
        </table>
      </div>

      {/* Pagination */}
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
    </div>
  );
}
