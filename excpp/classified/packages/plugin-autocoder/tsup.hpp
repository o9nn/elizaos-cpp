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
  outDir: 'dist',
  target: 'node20',
  format: ['esm'],
  external: ['@elizaos/core'],
  sourcemap: true,
  clean: true,
  dts: true,
});

} // namespace elizaos
