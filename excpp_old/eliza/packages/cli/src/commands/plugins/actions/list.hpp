#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".types.hpp"
#include ".utils/directory.hpp"
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * List available plugins from the registry
 */
std::future<void> listAvailablePlugins(ListPluginsOptions opts);

/**
 * List installed plugins in the current project
 */
std::future<void> listInstalledPlugins();

} // namespace elizaos
