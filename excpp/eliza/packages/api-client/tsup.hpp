#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

;

default defineConfig({
  entry: ['src/index.ts'],
  format: ['esm'],
  dts: {
    tsconfig: './tsconfig.build.json',
  },
  sourcemap: true,
  clean: true,
  external: ['@elizaos/core'],
  outDir: 'dist',
  esbuildOptions(options) {
    options.banner = {
      js: '"use client";',
    };
  },
});

} // namespace elizaos
