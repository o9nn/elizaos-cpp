#include "actions/plugin-creation-actions.hpp"
#include "elizaos/core.hpp"
#include "providers/plugin-creation-providers.hpp"
#include "services/plugin-creation-service.hpp"
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

;
;
;
;

// Export the plugin
const pluginDynamic: Plugin = {
  name: "@elizaos/plugin-dynamic",
  description: "Dynamic plugin creation system with AI-powered code generation",
  actions: [
    createPluginAction,
    checkPluginCreationStatusAction,
    cancelPluginCreationAction,
    createPluginFromDescriptionAction,
  ],
  providers: [pluginCreationStatusProvider, pluginCreationCapabilitiesProvider],
  services: [PluginCreationService],
  evaluators: [],
};

// Export individual components
{
  PluginCreationService,
  createPluginAction,
  checkPluginCreationStatusAction,
  cancelPluginCreationAction,
  createPluginFromDescriptionAction,
  pluginCreationStatusProvider,
  pluginCreationCapabilitiesProvider,
};

// Default default pluginDynamic;

// Re-types and utilities
{
  type PluginSpecification,
  type PluginCreationJob,
} from "./services/plugin-creation-service";
* from "./utils/plugin-templates";

} // namespace elizaos
