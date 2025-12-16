#include "..build-utils.hpp"
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
 * Build script for @elizaos/api-client using standardized build utilities
 */

;

// Create and run the standardized build runner
const run = createBuildRunner({
  packageName: '@elizaos/api-client',
  buildOptions: {
    entrypoints: ['src/index.ts'],
    outdir: 'dist',
    target: 'node',
    format: 'esm',
    external: ['@elizaos/core', 'fs', 'path'],
    sourcemap: true,
    minify: false,
    generateDts: true,
  },
  onBuildComplete: async (success) => {
    if (success) {
      // Re-all types and client from the proper dist structure
      const rootDtsContent = `// Main client
{ ElizaClient } from './client';

// Base types
* from './types/base';

// Domain types
* from './types/agents';
* from './types/messaging';
* from './types/memory';
* from './types/audio';
* from './types/media';
* from './types/server';
* from './types/system';
* from './types/sessions';
* from './types/runs';

// Services (for advanced usage)
{ AgentsService } from './services/agents';
{ MessagingService } from './services/messaging';
{ MemoryService } from './services/memory';
{ AudioService } from './services/audio';
{ MediaService } from './services/media';
{ ServerService } from './services/server';
{ SystemService } from './services/system';
{ SessionsService } from './services/sessions';
{ RunsService } from './services/runs';

// Base client and error
{ BaseApiClient, ApiError } from './lib/base-client';
`;
      await Bun.write('./dist/index.d.ts', rootDtsContent);
      console.log(' Created root index.d.ts');
    }
  },
});

// Execute the build
run().catch((error) => {
  console.error('Build script error:', error);
  process.exit(1);
});

} // namespace elizaos
