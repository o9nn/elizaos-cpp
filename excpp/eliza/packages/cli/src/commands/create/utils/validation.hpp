#pragma once
#include <algorithm>
#include <any>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "types.hpp"

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
  // Special case for current directory

/**
 * Processes and validates a plugin name.
 */
    // Remove common prefixes and suffixes

/**
 * Validates that the target directory is empty or doesn't exist.
 */


} // namespace elizaos
