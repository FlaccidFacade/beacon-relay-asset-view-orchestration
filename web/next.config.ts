import type { NextConfig } from "next";

const nextConfig: NextConfig = {
  basePath: "/bravo",
  output: "export",
  images: {
    unoptimized: true,
  },
};

export default nextConfig;
