#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
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
std::vector<Plugin> resolvePluginDependencies(const std::unordered_map<std::string, Plugin>& availablePlugins, bool isTestMode = false);

} // namespace elizaos
