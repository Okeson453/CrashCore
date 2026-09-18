"use client";

import clsx from "clsx";

type Props = {
  label: string;
  value: string;
  hint?: string;
  tone?: "default" | "positive" | "negative" | "warning" | "live";
};

export function KpiCard({ label, value, hint, tone = "default" }: Props) {
  return (
    <div className="rounded border border-surface-border bg-surface-panel px-3 py-2.5">
      <div className="text-[11px] uppercase tracking-wide text-ink-dim">{label}</div>
      <div
        className={clsx(
          "mt-1 font-mono text-xl tabular-nums leading-tight",
          tone === "positive" && "text-accent-positive",
          tone === "negative" && "text-accent-negative",
          tone === "warning" && "text-accent-warning",
          tone === "live" && "text-accent-live",
          tone === "default" && "text-ink"
        )}
      >
        {value}
      </div>
      {hint ? <div className="mt-0.5 text-[11px] text-ink-muted">{hint}</div> : null}
    </div>
  );
}
