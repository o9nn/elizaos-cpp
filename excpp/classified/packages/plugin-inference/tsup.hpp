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
  dts: true,
  sourcemap: true,
  clean: true,
  external: [
    '@elizaos/core',
    '@elizaos/plugin-openai',
    '@elizaos/plugin-anthropic',
    '@elizaos/plugin-ollama',
    '@elizaos/plugin-elizaos-services',
    '@elizaos/plugin-groq',
    '@elizaos/plugin-local-embedding',
  ],
});

} // namespace elizaos
