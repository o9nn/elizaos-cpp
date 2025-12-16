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
  clean: false,
  format: ['esm'], // Ensure you're targeting CommonJS
  dts: false, // Skip DTS generation to avoid external import issues // Ensure you're targeting CommonJS
  external: [
    'dotenv', // Externalize dotenv to prevent bundling
    'fs', // Externalize fs to use Node.js built-in module
    'fs-extra', // Externalize fs-extra to prevent bundling issues
    'graceful-fs', // Externalize graceful-fs (dependency of fs-extra)
    'path', // Externalize other built-ins if necessary
    'https',
    'http',
    'zod',
    '@jitsi/robotjs', // Externalize native modules
    'canvas', // Externalize native modules
    'sharp', // Externalize native modules
  ],
});

} // namespace elizaos
