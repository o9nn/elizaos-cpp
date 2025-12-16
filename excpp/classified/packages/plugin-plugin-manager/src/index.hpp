#include "actions/clonePluginAction.hpp"
#include "actions/installPluginFromRegistry.hpp"
#include "actions/loadPlugin.hpp"
#include "actions/publishPluginAction.hpp"
#include "actions/searchPluginAction.hpp"
#include "actions/unloadPlugin.hpp"
#include "elizaos/core.hpp"
#include "providers/pluginConfigurationStatus.hpp"
#include "providers/pluginKnowledgeProvider.hpp"
#include "providers/pluginStateProvider.hpp"
#include "providers/registryPluginsProvider.hpp"
#include "services/pluginConfigurationService.hpp"
#include "services/pluginManagerService.hpp"
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
;
;
;
;
;
;
;
;
;
import './types'; // Ensure module augmentation is loaded
;

/**
 * Plugin Manager Plugin for ElizaOS
 *
 * Provides comprehensive plugin management capabilities including:
 * - Dynamic loading and unloading of plugins at runtime
 * - Plugin registry integration with caching for discovering and installing plugins
 * - Basic configuration management for environment variable checking
 * - Plugin search, clone, and publish functionality
 *
 * Core Features:
 * - Search plugins using cached registry data
 * - Install plugins from registry
 * - Load and unload plugins dynamically
 * - Clone plugins for development
 * - Publish plugins to npm registry
 * - Check plugin configuration status and missing API keys
 *
 * IMPORTANT: The agent should use this plugin manager to:
 * - Search for and discover new plugins from the registry
 * - Install plugins to extend its capabilities
 * - Clone plugins for modification and development
 * - Load/unload non-protected plugins as needed
 *
 * Protected plugins (loaded at startup) CANNOT be unloaded or modified
 * to ensure system stability and core functionality.
 */
const pluginManagerPlugin: Plugin = {
  name: 'plugin-manager',
  description:
    'Manages dynamic loading and unloading of plugins at runtime, with registry integration and configuration status checking',

  services: [PluginManagerService, PluginConfigurationService],

  actions: [
    loadPluginAction,
    unloadPluginAction,
    installPluginFromRegistryAction,
    searchPluginAction,
    getPluginDetailsAction,
    clonePluginAction,
    publishPluginAction,
  ],

  providers: [
    pluginStateProvider,
    pluginConfigurationStatusProvider,
    registryPluginsProvider,
    pluginKnowledgeProvider,
  ],

  evaluators: [],

  routes: [],

  init: async (config: Record<string, any>, runtime: IAgentRuntime) => {
    // Service initialization is handled by the runtime when calling static start()
    // No need to manually initialize here
  },
};

} // namespace elizaos
