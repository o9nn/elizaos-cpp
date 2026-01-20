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
#include "services/pluginManagerService.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



    // Extract plugin name from message content

    // Try different patterns to extract plugin name
    // Pattern 1: install [plugin] from registry <name>

    // Pattern 2: install [plugin] <name> [from registry]

    // Pattern 3: add/download/get plugin <name>

    // Check for install/registry-related keywords

    // Look for plugin name patterns

      // Look for "install [name] plugin" pattern

    // Valid if it's an install command with plugin context

} // namespace elizaos
