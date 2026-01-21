#include "actions/server-start.hpp"
#include "elizaos/core.hpp"
#include "elizaos/server.hpp"
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
