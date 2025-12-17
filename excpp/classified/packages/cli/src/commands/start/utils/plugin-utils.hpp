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
#include "elizaos/core.hpp"
#include "types.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Check if an object has a valid plugin shape
 */

/**
 * Load and prepare a plugin for use
 *
 * Handles both local development plugins and published plugins, with automatic installation if needed.
 */

/**
 * Validate a plugin object
 */
PluginValidation validatePlugin(const std::any& plugin);

} // namespace elizaos
