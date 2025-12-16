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
  entry: ['src/index.ts', 'src/specs/v1/*.ts', 'src/specs/v2/index.ts'],
  outDir: 'dist',
  clean: true,
  format: ['esm'],
  target: 'node18',
  dts: true,
  tsconfig: './tsconfig.build.json', // Use build-specific tsconfig
  ignoreWatch: ['**/__tests__/**', '**/test_resources/**'],
  external: [
    'dotenv',
    'fs',
    'path',
    'node:fs',
    'node:path',
    'node:crypto',
    'node:web',
    'node:stream',
    'node:buffer',
    'node:util',
    'node:events',
    'node:url',
    'node:http',
    'node:https',
    'http',
    'https',
    'sharp',
    '@solana/web3.js',
    'zod',
    '@hapi/shot',
  ],
  sourcemap: false,
});

} // namespace elizaos
