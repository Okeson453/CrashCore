import type { StatisticsUpdate, ConnectionState } from "@/types/statistics";

export type StreamHandlers = {
  onUpdate: (u: StatisticsUpdate) => void;
  onConnection: (s: ConnectionState) => void;
  onError?: (msg: string) => void;
};

/**
 * Subscribe to live statistics.
 * Prefer native WebSocket to CrashCore; fall back to HTTP poll of /api/statistics/overview.
 * Demo interval only when NEXT_PUBLIC_FORCE_DEMO=true (never marks LIVE with fake numbers).
 */
export function connectStatisticsStream(handlers: StreamHandlers): () => void {
  let closed = false;
  let ws: WebSocket | null = null;
  let pollTimer: ReturnType<typeof setInterval> | null = null;
  let retryTimer: ReturnType<typeof setTimeout> | null = null;
  let attempt = 0;

  const forceDemo = process.env.NEXT_PUBLIC_FORCE_DEMO === "true";

  function setConn(s: ConnectionState) {
    if (!closed) handlers.onConnection(s);
  }

  function startHttpPoll() {
    if (pollTimer) return;
    setConn("DEGRADED");
    const tick = async () => {
      if (closed) return;
      try {
        const res = await fetch("/api/statistics/overview", {
          cache: "no-store",
          signal: AbortSignal.timeout(4000),
        });
        if (!res.ok) {
          setConn("DISCONNECTED");
          handlers.onError?.(`poll HTTP ${res.status}`);
          return;
        }
        if (res.headers.get("X-CrashCore-Source") === "offline") {
          setConn("DISCONNECTED");
          return;
        }
        const data = await res.json();
        if (data.__source === "demo") {
          setConn("DISCONNECTED");
          return;
        }
        setConn("LIVE");
        handlers.onUpdate({
          type: "statistics_update",
          timestamp: data.updatedAt || new Date().toISOString(),
          sequence: data.sequence ?? 0,
          rounds: data.totalRounds ?? 0,
          predictions: data.totalPredictions ?? 0,
          wins: data.wins ?? 0,
          losses: data.losses ?? 0,
          winRate: data.winRate ?? 0,
          latencyP95: data.latencyP95 ?? data.p95 ?? 0,
        });
      } catch (e) {
        setConn("DISCONNECTED");
        handlers.onError?.(e instanceof Error ? e.message : String(e));
      }
    };
    void tick();
    pollTimer = setInterval(tick, 5000);
  }

  function connectWs() {
    if (closed || forceDemo) {
      if (forceDemo) {
        setConn("DISCONNECTED");
        handlers.onError?.("FORCE_DEMO: not connecting");
      }
      startHttpPoll();
      return;
    }

    const wsUrl = process.env.NEXT_PUBLIC_STATS_WS_URL;
    if (!wsUrl) {
      // No dedicated WebSocket URL configured; use robust HTTP polling
      startHttpPoll();
      return;
    }

    try {
      ws = new WebSocket(wsUrl);
    } catch {
      startHttpPoll();
      return;
    }

    ws.onopen = () => {
      attempt = 0;
      setConn("LIVE");
    };
    ws.onmessage = (ev) => {
      try {
        const data = JSON.parse(String(ev.data));
        handlers.onUpdate({
          type: "statistics_update",
          timestamp: data.timestamp || data.updatedAt || new Date().toISOString(),
          sequence: data.sequence ?? 0,
          rounds: data.rounds ?? data.totalRounds ?? 0,
          predictions: data.predictions ?? data.totalPredictions ?? 0,
          wins: data.wins ?? 0,
          losses: data.losses ?? 0,
          winRate: data.winRate ?? 0,
          latencyP95: data.latencyP95 ?? data.p95 ?? 0,
        });
        setConn("LIVE");
      } catch {
        /* ignore malformed */
      }
    };
    ws.onerror = () => {
      handlers.onError?.("websocket error");
    };
    ws.onclose = () => {
      setConn("DISCONNECTED");
      ws = null;
      if (closed) return;
      // Fallback to HTTP poll + backoff reconnect
      startHttpPoll();
      const delay = Math.min(30_000, 1000 * Math.pow(2, attempt++));
      retryTimer = setTimeout(() => {
        if (pollTimer) {
          clearInterval(pollTimer);
          pollTimer = null;
        }
        connectWs();
      }, delay);
    };
  }

  connectWs();

  return () => {
    closed = true;
    if (retryTimer) clearTimeout(retryTimer);
    if (pollTimer) clearInterval(pollTimer);
    ws?.close();
  };
}
