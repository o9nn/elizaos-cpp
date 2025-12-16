#include "utils/build-utils.hpp"
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

#!/usr/bin/env bun
/**
 * Build script for @elizaos/plugin-bootstrap using standardized build utilities
 */

;

// Create and run the standardized build runner
const run = createBuildRunner({
  packageName: '@elizaos/plugin-bootstrap',
  buildOptions: {
    entrypoints: ['src/index.ts'],
    outdir: 'dist',
    target: 'node',
    format: 'esm',
    external: [
      'dotenv',
      'fs',
      'path',
      '@reflink/reflink',
      'agentkeepalive',
      'zod',
      '@elizaos/core',
      '@elizaos/plugin-sql',
    ],
    sourcemap: true,
    minify: false,
    generateDts: true,
  },
});

// Execute the build
run().catch((error) => {
  console.error('Build script error:', error);
  process.exit(1);
});

} // namespace elizaos
