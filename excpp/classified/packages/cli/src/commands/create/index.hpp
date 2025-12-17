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



/**
 * Formats the project type for display in messages
 */
std::string formatProjectType(const std::string& type);

      // Validate and parse options

      // If no name provided, prompt for type first then name

        // Show intro message after type is determined

        // Prompt for name

              // Validate project/plugin names differently than agent names

      // Validate project name for non-agent types

      // Show intro message now that we have both type and name

      // Handle different project types

          // TEE projects need database and AI model selection

            // Check if selected AI model needs embedding model fallback

          // Regular projects need database and AI model selection

            // Check if selected AI model needs embedding model fallback

        // Dynamic outro message based on project type
        // Dynamic error message based on project type

} // namespace elizaos
