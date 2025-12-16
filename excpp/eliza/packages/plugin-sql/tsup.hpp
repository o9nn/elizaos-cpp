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
  entry: ['src/index.ts', 'src/migrate.ts'],
  outDir: 'dist',
  sourcemap: true,
  clean: true,
  format: ['esm'], // Ensure you're targeting CommonJS
  dts: true,
  tsconfig: './tsconfig.build.json', // Use build-specific tsconfig
  external: [
    'dotenv', // Externalize dotenv to prevent bundling
    '@reflink/reflink',
    '@node-llama-cpp',
    'https',
    'http',
    'agentkeepalive',
    'uuid',
    '@elizaos/core',
    '@electric-sql/pglite',
    'zod',
  ],
  // Improve source map configuration
  esbuildOptions(options) {
    options.sourceRoot = './'; // Set source root to help with source mapping
    options.sourcesContent = true;
    options.outbase = './src'; // Makes output paths match input structure
  },
  keepNames: true, // Preserve names for better debugging
});

} // namespace elizaos
