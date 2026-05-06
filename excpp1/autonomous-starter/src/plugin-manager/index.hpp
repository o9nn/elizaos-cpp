#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "actions/installPluginFromRegistry.js.hpp"
#include "actions/loadPlugin.hpp"
#include "actions/startPluginConfiguration.hpp"
#include "actions/unloadPlugin.hpp"
#include "elizaos/core.hpp"
#include "evaluators/pluginConfigurationEvaluator.hpp"
#include "providers/pluginConfigurationStatus.hpp"
#include "providers/pluginStateProvider.hpp"
#include "providers/registryPluginsProvider.hpp"
#include "services/pluginConfigurationService.hpp"
#include "services/pluginManagerService.hpp"
#include "services/pluginUserInteractionService.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Plugin Manager Plugin for ElizaOS
 *
 * Provides comprehensive plugin management capabilities including:
 * - Dynamic loading and unloading of plugins at runtime
 * - Plugin registry integration for discovering and installing plugins
 * - Secure configuration management with encrypted storage
 * - Interactive dialog system for collecting environment variables
 * - Proactive configuration suggestions and status monitoring
 *
 * Features:
 * - Registry-based plugin discovery and installation
 * - Dynamic plugin loading/unloading without restart
 * - Secure environment variable management with AES-256-CBC encryption
 * - Interactive user dialogs for plugin configuration
 * - Package.json convention for declaring required variables
 * - Validation and secure storage mechanisms
 * - Agent behavior integration for proactive configuration
 * - Complete testing and validation pipeline
 */

    // Any initialization logic if needed

// Export all plugin manager components
* from "./types";
* from "./services/pluginManagerService";
* from "./services/pluginConfigurationService";
* from "./services/pluginUserInteractionService";
* from "./actions/loadPlugin";
* from "./actions/unloadPlugin";
* from "./actions/startPluginConfiguration";
* from "./actions/installPluginFromRegistry";
* from "./providers/pluginStateProvider";
* from "./providers/pluginConfigurationStatus";
* from "./providers/registryPluginsProvider";
* from "./evaluators/pluginConfigurationEvaluator";

} // namespace elizaos
