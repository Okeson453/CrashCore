import type { NextConfig } from "next";

/**
 * CRASHCORE_API_URL — origin of C++ StatisticsServer (default http://127.0.0.1:8080).
 * App Router handlers under src/app/api/statistics/[...path] proxy GET-only requests.
 * Rewrites remain as a secondary path for static export edge cases.
 */
const nextConfig: NextConfig = {
  reactStrictMode: true,
  output: "standalone",
};

export default nextConfig;

