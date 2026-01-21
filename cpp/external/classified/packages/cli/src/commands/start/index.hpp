#include "actions/server-start.hpp"
#include "elizaos/core.hpp"
#include "types.hpp"
#include "utils/config-utils.hpp"
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



    await displayBanner();
      // Load env config first before any character loading
      await loadEnvConfig();

      // Setup proper module resolution environment variables
      // This ensures consistent plugin loading between dev and start commands

      // Add local .bin to PATH to prioritize local executables

      // Build the project first (unless it's a monorepo)

          // Use buildProject function with proper UI feedback and error handling
          await buildProject(cwd, false);

        // Load @elizaos/server module for character loading

        // Validate and load characters from provided paths

        // Try to load project agents if no character files specified

          // Check if we're in a directory that might contain agents - allow any directory with package.json
          // except those explicitly detected as non-ElizaOS (covers projects, plugins, monorepos, etc.)

              // Log loaded agent names

      await startAgents({ ...options, characters, projectAgents });

// Re-for backward compatibility
* from './actions/agent-start';
* from './actions/server-start';
* from './types';
* from './utils/config-utils';
* from './utils/dependency-resolver';
* from './utils/plugin-utils';

} // namespace elizaos
