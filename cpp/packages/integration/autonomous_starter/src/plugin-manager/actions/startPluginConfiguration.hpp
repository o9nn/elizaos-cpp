#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <future>
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



      // Check if plugin configuration service is available

      // Check if interaction service is available

      // Check for configuration-related keywords

      // Extract plugin name from the message

      // Check if plugin exists and get its requirements

      // Check current configuration status - find which variables are missing

      // Create configuration request

      // Start the configuration dialog using agentId as a fallback for userId

      // Generate the first prompt

std::future<std::string> extractPluginNameFromMessage(IAgentRuntime runtime, const std::string& text);

} // namespace elizaos
