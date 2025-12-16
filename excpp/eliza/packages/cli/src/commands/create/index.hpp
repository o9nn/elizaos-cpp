#include "actions.hpp"
#include "elizaos/core.hpp"
#include "utils.hpp"
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



      // Set non-interactive mode if environment variable is set or if -y/--yes flag is present in process.argv

      // Validate and parse options

        await displayBanner();

      // If no name provided, prompt for type first then name

        // Prompt for name

              // Validate project/plugin names differently than agent names

      // Validate project name for non-agent types

      // Handle different project types
          await createPlugin(projectName!, targetDir, isNonInteractive);

          await createAgent(projectName!, targetDir, isNonInteractive);

          // TEE projects need database and AI model selection

            // Check if selected AI model needs embedding model fallback

          // Regular projects need database and AI model selection

            // Check if selected AI model needs embedding model fallback


} // namespace elizaos
