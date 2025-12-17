#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Resolve plugin dependencies with circular dependency detection
 *
 * Performs topological sorting of plugins to ensure dependencies are loaded in the correct order, with support for test dependencies.
 */
std::vector<Plugin> resolvePluginDependencies(Map<string availablePlugins, auto Plugin>, boolean = false isTestMode);

} // namespace elizaos
