"use client";

import { ModelsTable } from "@/components/tables/ModelsTable";
import { useModels } from "@/data/queries/use-statistics";

export default function ModelsPage() {
  const { data, isLoading } = useModels();

  if (isLoading && !data) return <p className="text-sm text-ink-muted">Loading model statistics…</p>;
  if (!data) return <p className="text-sm text-ink-muted">No model statistics.</p>;

  return (
    <div className="space-y-5">
      <div>
        <h1 className="text-lg font-medium">Model Statistics</h1>
        <p className="text-xs text-ink-dim">
          Observed performance by model · no ranking or selection actions
        </p>
      </div>
      <ModelsTable rows={data} />
    </div>
  );
}
