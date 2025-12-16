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
  clean: true,
  format: ['esm'],
  target: 'node18',
  dts: true,
  tsconfig: './tsconfig.build.json',
  // No need for source maps or watching for a config package
  sourcemap: false,
  // Skip minification to keep the exported paths readable
  minify: false,
  // Don't bundle - this package just re-exports configs
  bundle: false,
});

} // namespace elizaos
