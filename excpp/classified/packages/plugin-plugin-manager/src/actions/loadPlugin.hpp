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



    // Check if the message is asking to load a plugin

    // Check if there are any plugins that can be loaded

    // Extract plugin name from message

    // Find plugin to load

    // First try exact match

    // If no exact match, get the first loadable plugin

    // Check for missing environment variables


} // namespace elizaos
