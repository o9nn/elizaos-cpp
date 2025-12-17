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
#include "services/pluginRegistryService.js.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



    // Validation patterns

    // Extract search query from message

      // Search using the API-based registry service

      // Format results with rich information

      // Add helpful suggestions

/**
 * Extract search query from user message using improved patterns
 */
std::string extractSearchQuery(const std::string& text);

/**
 * Helper action to get plugin details
 */

      // Remove features and requiredConfig - these properties don't exist on PluginMetadata

      // Remove npmPackage - this property doesn't exist on PluginMetadata

      // Add related plugins if available
      // This functionality will be re-enabled once the API supports it
      // if (details.relatedPlugins && details.relatedPlugins.length > 0) {
      //   responseText += `\n💡 **Related Plugins:**\n${details.relatedPlugins.map((p) => `• ${p.name} (${p.reason})`).join('\n')}`;
      // }


} // namespace elizaos
