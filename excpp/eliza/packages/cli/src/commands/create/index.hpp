#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "actions.hpp"
#include "elizaos/core.hpp"
#include "utils.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



      // Validate and parse options

      // If no name provided, prompt for type first then name

        // Prompt for name

              // Validate project/plugin names differently than agent names

      // Validate project name for non-agent types

      // Handle different project types

          // TEE projects need database and AI model selection

            // Check if selected AI model needs embedding model fallback

          // Regular projects need database and AI model selection

            // Check if selected AI model needs embedding model fallback


} // namespace elizaos
