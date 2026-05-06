#include ".types.hpp"
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



/**
 * Project name validation schema
 */

/**
 * Plugin name validation schema
 */

/**
 * Validates create command options using Zod schema
 */
CreateOptions validateCreateOptions(const std::any& options);

/**
 * Validates a project name according to npm package naming rules.
 * Special case: "." is allowed for current directory projects.
 */
void validateProjectName(const std::string& name); {
  // Special case for current directory

/**
 * Processes and validates a plugin name.
 */
void processPluginName(const std::string& name); {
    // Remove common prefixes and suffixes

/**
 * Validates that the target directory is empty or doesn't exist.
 */


} // namespace elizaos
