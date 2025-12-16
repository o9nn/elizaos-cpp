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
  tsconfig: './tsconfig.build.json', // Use build-specific tsconfig
  sourcemap: true,
  clean: false, // Don't clean dist folder before building
  format: ['esm'], // Ensure you're targeting CommonJS
  dts: true, // Generate TypeScript declaration files (.d.ts) for better type support
  external: [
    'dotenv', // Externalize dotenv to prevent bundling
    'node:fs', // Externalize fs to use Node.js built-in module
    'node:https',
    'node:path', // Externalize other built-ins if necessary
    'node:http',
    '@elizaos/plugin-cli',
    '@elizaos/core',
    'zod',
  ],
});

} // namespace elizaos
