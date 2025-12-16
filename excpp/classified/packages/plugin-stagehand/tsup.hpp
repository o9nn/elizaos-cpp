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
  format: ['esm'],
  sourcemap: true,
  clean: true,
  dts: true,
  external: [
    // Mark browser automation dependencies as external
    '@browserbasehq/stagehand',
    'playwright',
    'playwright-core',
    'playwright-chromium',
    'playwright-firefox',
    'playwright-webkit',
    '@playwright/test',
    'electron',
    // Also mark native dependencies as external
    'canvas',
    '@napi-rs/canvas',
    'bufferutil',
    'utf-8-validate',
  ],
  noExternal: [
    // Bundle our own modules
    './websocket-client',
    './process-manager',
    './errors',
    './retry',
    './security',
    './plugin',
  ],
});

} // namespace elizaos
