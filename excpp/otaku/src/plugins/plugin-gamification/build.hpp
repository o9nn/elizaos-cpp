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
 * Build script for @elizaos/plugin-gamification
 */

;

const result = await build({
  entrypoints: ['src/index.ts'],
  outdir: 'dist',
  target: 'node',
  format: 'esm',
  external: [
    '@elizaos/core',
    '@elizaos/plugin-sql',
    'drizzle-orm',
    'uuid',
  ],
  sourcemap: true,
  minify: false,
});

if (result.success) {
  console.log('✓ Build successful');
} else {
  console.error('✗ Build failed');
  process.exit(1);
}


} // namespace elizaos
