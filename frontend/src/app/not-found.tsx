import Link from "next/link";
import { AlertTriangle, ArrowLeft } from "lucide-react";

export default function NotFound() {
  return (
    <div className="flex min-h-[60vh] flex-col items-center justify-center text-center">
      <div className="rounded-full bg-accent-amber/10 p-4 text-accent-amber mb-4 border border-accent-amber/20">
        <AlertTriangle className="h-8 w-8" />
      </div>
      <h2 className="text-xl font-bold font-mono text-ink">404 - Telemetry View Not Found</h2>
      <p className="mt-2 text-xs text-ink-dim max-w-md font-mono">
        The requested telemetry endpoint or analytics page could not be located on this observatory node.
      </p>
      <Link
        href="/"
        className="mt-6 inline-flex items-center gap-2 rounded-lg border border-accent-cyan/40 bg-accent-cyan/15 px-4 py-2 font-mono text-xs font-semibold text-accent-cyan hover:bg-accent-cyan/25 transition-colors"
      >
        <ArrowLeft className="h-3.5 w-3.5" />
        Return to Overview Telemetry
      </Link>
    </div>
  );
}
