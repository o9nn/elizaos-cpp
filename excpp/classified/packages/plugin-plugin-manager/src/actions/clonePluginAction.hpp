#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".services/pluginManagerService.hpp"
#include ".services/pluginRegistryService.js.hpp"
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



    // Extract plugin name from message

    // Clone the plugin using API-based registry service

    // Prepare response with helpful information

    // Optionally register the cloned plugin for development

std::string extractPluginName(const std::string& text);

} // namespace elizaos
