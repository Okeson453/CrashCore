"use client";

import { useState } from "react";
import { RoundsTable } from "@/components/tables/RoundsTable";
import { useRounds } from "@/data/queries/use-statistics";
import type { RoundsFilter } from "@/types/statistics";
import { fmtNum } from "@/lib/formatting/numbers";

export default function DataPage() {
  const [filter, setFilter] = useState<RoundsFilter>({ limit: 50 });
  const { data, isLoading, isFetching } = useRounds(filter);

  return (
    <div className="space-y-5">
      <div>
        <h1 className="text-lg font-medium">Historical Data</h1>
        <p className="text-xs text-ink-dim">
          Server-side filtered rounds · cursor pagination · no local mutation
        </p>
      </div>

      <form
        className="grid gap-2 rounded border border-surface-border bg-surface-panel p-3 sm:grid-cols-2 lg:grid-cols-4"
        onSubmit={(e) => {
          e.preventDefault();
          const fd = new FormData(e.currentTarget);
          setFilter({
            limit: 50,
            regime: String(fd.get("regime") || "") || undefined,
            from: String(fd.get("from") || "") || undefined,
            to: String(fd.get("to") || "") || undefined,
            cursor: undefined,
          });
        }}
      >
        <label className="text-xs text-ink-dim">
          From
          <input
            name="from"
            type="datetime-local"
            className="mt-1 w-full rounded border border-surface-border bg-surface px-2 py-1.5 text-sm text-ink"
          />
        </label>
        <label className="text-xs text-ink-dim">
          To
          <input
            name="to"
            type="datetime-local"
            className="mt-1 w-full rounded border border-surface-border bg-surface px-2 py-1.5 text-sm text-ink"
          />
        </label>
        <label className="text-xs text-ink-dim">
          Regime
          <select
            name="regime"
            className="mt-1 w-full rounded border border-surface-border bg-surface px-2 py-1.5 text-sm text-ink"
            defaultValue=""
          >
            <option value="">All</option>
            <option>High Activity</option>
            <option>Low Activity</option>
            <option>Volatile</option>
            <option>Stable</option>
          </select>
        </label>
        <div className="flex items-end gap-2">
          <button
            type="submit"
            className="rounded border border-surface-border bg-surface-raised px-3 py-1.5 text-sm text-ink hover:border-ink-dim"
          >
            Apply filters
          </button>
          {isFetching ? <span className="text-xs text-ink-dim">Refreshing…</span> : null}
        </div>
      </form>

      <div className="text-xs text-ink-muted">
        {data ? (
          <>
            Showing {data.items.length} of {fmtNum(data.total)} rounds
          </>
        ) : isLoading ? (
          "Loading…"
        ) : (
          "No rows"
        )}
      </div>

      {data ? <RoundsTable rows={data.items} /> : null}

      {data?.nextCursor ? (
        <button
          type="button"
          className="rounded border border-surface-border bg-surface-raised px-3 py-1.5 text-sm text-ink hover:border-ink-dim"
          onClick={() => setFilter((f) => ({ ...f, cursor: data.nextCursor ?? undefined }))}
        >
          Load more
        </button>
      ) : null}
    </div>
  );
}
