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
#include "services/pluginConfigurationService.hpp"
#include "services/pluginUserInteractionService.hpp"
#include "types.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



      // Check if plugin configuration services are available

      // Run if message mentions plugins, configuration, or setup

      // Check for unconfigured plugins

            // Skip plugins that can't be parsed

      // Check if there are active configuration dialogs

      // Generate evaluation response based on context

      // If user mentioned specific plugin names, focus on those

        // General suggestion when there are unconfigured plugins
        // Remind about ongoing configuration dialogs

      // Special handling for installation/setup messages


} // namespace elizaos
