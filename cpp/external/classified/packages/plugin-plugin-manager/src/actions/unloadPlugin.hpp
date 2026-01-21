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
#include "services/pluginManagerService.ts.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



    // Check if the message is asking to unload a plugin

    // Check if there are any plugins that can be unloaded

    // Filter out protected plugins that cannot be unloaded

    // Extract plugin name from message

    // Find plugin to unload

    // First try exact match

    // If no exact match, provide a list of loaded plugins

    // Check if plugin can be unloaded


} // namespace elizaos
