# CrashCore Frontend — Statistics Observatory

Read-only observability UI for CrashCore. **Never** generates predictions, mutates config, or controls workers.

## Stack

- Next.js 15 / React 19 / TypeScript
- Tailwind CSS (dark quantitative theme)
- TanStack Query + Table
- Zustand (live stream patch only)
- Recharts

## Pages

| Route | Content |
|-------|---------|
| `/` | Overview KPIs |
| `/predictions` | Prediction metrics + table |
| `/performance` | Equity, drawdown, rolling win rate |
| `/models` | Per-model observed stats |
| `/regimes` | Regime-stratified stats |
| `/latency` | Critical-path latency |
| `/data` | Historical rounds (filtered, paginated) |

## Run

```bash
cd frontend
npm install
npm run dev
```

Open http://localhost:3000

When the CrashCore Statistics API is unavailable, the client serves **demo mock data** so the UI remains fully functional.

## API (GET only)

```
GET /api/statistics/overview
GET /api/statistics/predictions
GET /api/statistics/performance
GET /api/statistics/models
GET /api/statistics/regimes
GET /api/statistics/latency
GET /api/statistics/rounds
GET /api/statistics/health
WS  /api/statistics/stream
```

Configure backend origin via `CRASHCORE_API_URL` / `NEXT_PUBLIC_WS_URL`.

## Hard boundary

- No POST/PUT/PATCH/DELETE from this app
- No prediction, betting, or worker control UI
- Engine owns truth; frontend owns presentation
