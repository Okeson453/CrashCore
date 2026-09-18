import { NextRequest, NextResponse } from "next/server";

const BACKEND = process.env.CRASHCORE_API_URL || "http://127.0.0.1:8080";

/** Read-only proxy: only GET/OPTIONS forwarded to CrashCore StatisticsServer. */
export async function GET(
  _req: NextRequest,
  ctx: { params: Promise<{ path: string[] }> },
) {
  const { path } = await ctx.params;
  const sub = path?.join("/") ?? "";
  const url = `${BACKEND}/api/statistics/${sub}${_req.nextUrl.search}`;

  try {
    const res = await fetch(url, {
      method: "GET",
      headers: { Accept: "application/json" },
      cache: "no-store",
      signal: AbortSignal.timeout(4000),
    });
    const text = await res.text();
    return new NextResponse(text, {
      status: res.status,
      headers: {
        "Content-Type": res.headers.get("Content-Type") || "application/json",
        "X-CrashCore-Source": "backend",
        "Cache-Control": "no-store",
      },
    });
  } catch (e) {
    const msg = e instanceof Error ? e.message : "backend unreachable";
    return NextResponse.json(
      {
        error: msg,
        __source: "demo",
        state: "DISCONNECTED",
        summary: "CrashCore backend not reachable on " + BACKEND,
      },
      {
        status: 503,
        headers: {
          "X-CrashCore-Source": "offline",
          "Cache-Control": "no-store",
        },
      },
    );
  }
}

export async function OPTIONS() {
  return new NextResponse(null, {
    status: 204,
    headers: {
      "Access-Control-Allow-Methods": "GET, OPTIONS",
      "Access-Control-Allow-Headers": "Accept, Content-Type",
    },
  });
}

/** Reject all mutation verbs — statistics observatory is read-only. */
export async function POST() {
  return NextResponse.json({ error: "read-only" }, { status: 405 });
}
export async function PUT() {
  return NextResponse.json({ error: "read-only" }, { status: 405 });
}
export async function PATCH() {
  return NextResponse.json({ error: "read-only" }, { status: 405 });
}
export async function DELETE() {
  return NextResponse.json({ error: "read-only" }, { status: 405 });
}
