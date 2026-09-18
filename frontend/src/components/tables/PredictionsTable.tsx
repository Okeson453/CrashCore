"use client";

import {
  useReactTable,
  getCoreRowModel,
  flexRender,
  createColumnHelper,
} from "@tanstack/react-table";
import type { PredictionRow } from "@/types/statistics";
import { fmtPct, fmtMs, fmtMult } from "@/lib/formatting/numbers";
import { useMemo } from "react";

const col = createColumnHelper<PredictionRow>();

export function PredictionsTable({ rows }: { rows: PredictionRow[] }) {
  const columns = useMemo(
    () => [
      col.accessor("gameId", { header: "Game ID" }),
      col.accessor("timestamp", {
        header: "Timestamp",
        cell: (c) => new Date(c.getValue()).toLocaleString(undefined, { hour12: false }),
      }),
      col.accessor("target", { header: "Target", cell: (c) => fmtMult(c.getValue()) }),
      col.accessor("probability", { header: "Prob", cell: (c) => fmtPct(c.getValue()) }),
      col.accessor("confidence", { header: "Conf", cell: (c) => fmtPct(c.getValue()) }),
      col.accessor("edge", { header: "Edge", cell: (c) => fmtPct(c.getValue()) }),
      col.accessor("regime", { header: "Regime" }),
      col.accessor("model", { header: "Model" }),
      col.accessor("decision", { header: "Decision" }),
      col.accessor("actualResult", {
        header: "Result",
        cell: (c) => {
          const v = c.getValue();
          const cls =
            v === "Win" ? "text-accent-positive" : v === "Loss" ? "text-accent-negative" : "text-ink-muted";
          return <span className={cls}>{v}</span>;
        },
      }),
      col.accessor("latencyMs", { header: "Latency", cell: (c) => fmtMs(c.getValue()) }),
    ],
    []
  );

  const table = useReactTable({ data: rows, columns, getCoreRowModel: getCoreRowModel() });

  return (
    <div className="overflow-x-auto rounded border border-surface-border">
      <table className="w-full min-w-[900px] text-left text-xs">
        <thead className="bg-surface-raised text-ink-dim">
          {table.getHeaderGroups().map((hg) => (
            <tr key={hg.id}>
              {hg.headers.map((h) => (
                <th key={h.id} className="px-2 py-2 font-medium">
                  {flexRender(h.column.columnDef.header, h.getContext())}
                </th>
              ))}
            </tr>
          ))}
        </thead>
        <tbody>
          {table.getRowModel().rows.map((row) => (
            <tr key={row.id} className="border-t border-surface-border/60 hover:bg-surface-raised/40">
              {row.getVisibleCells().map((cell) => (
                <td key={cell.id} className="px-2 py-1.5 font-mono tabular-nums text-ink">
                  {flexRender(cell.column.columnDef.cell, cell.getContext())}
                </td>
              ))}
            </tr>
          ))}
        </tbody>
      </table>
    </div>
  );
}
