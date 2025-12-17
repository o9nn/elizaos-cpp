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
#include "services/pluginManagerService.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



    // Check if there are any plugins that can be unloaded

    // We can't unload original plugins, so check if there are any non-original loaded plugins
    // For now, we'll assume any plugin can potentially be unloaded (the service will check)

      // Extract plugin name from message

      // Find plugin to unload

      // First try exact match

      // If no exact match, provide a list of loaded plugins

      // Check if it's because it's an original plugin


} // namespace elizaos
