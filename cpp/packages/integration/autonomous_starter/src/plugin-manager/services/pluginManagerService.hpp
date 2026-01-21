#include ".types.hpp"
#include "elizaos/core.hpp"
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



// Registry installation types and functions
struct RegistryEntry {
    std::string name;
    std::optional<std::string> description;
    std::string repository;
    std::optional<{> npm;
    std::string repo;
    std::optional<std::string> v1;
    std::optional<{> git;
    std::string repo;
    std::optional<{> v1;
    std::optional<std::string> branch;
    std::optional<std::string> version;
};

struct DynamicPluginInfo {
    std::string name;
    std::string version;
    std::string path;
    Array<{ requiredEnvVars;
    std::string name;
    std::string description;
    bool sensitive;
    bool isSet;
    std::optional<std::string> errorDetails;
    Date installedAt;
    std::optional<Date> lastActivated;
};

// Function to reset cache for testing
void resetRegistryCache();

// Registry functions

// Real plugin installation function using npm/git
std::future<void> installPlugin(const std::string& pluginName, const std::string& targetDir, std::optional<std::string> version);

// Install plugin from npm
std::future<void> installFromNpm(const std::string& packageName, const std::string& version, const std::string& targetDir);

// Install plugin from git repository
std::future<void> installFromGit(const std::string& gitRepo, const std::string& version, const std::string& targetDir);

class PluginManagerService extends Service implements PluginRegistry {
  static override serviceType: ServiceTypeName =
    PluginManagerServiceType.PLUGIN_MANAGER;
  override capabilityDescription =
    "Manages dynamic loading and unloading of plugins at runtime, including registry installation";

  public plugins: Map<string, PluginState> = new Map();
  private pluginManagerConfig: PluginManagerConfig;
  private originalPlugins: Plugin[] = [];
  private originalActions: Set<string> = new Set();
  private originalProviders: Set<string> = new Set();
  private originalEvaluators: Set<string> = new Set();
  private originalServices: Set<string> = new Set();

  // Add registry installation state management
  private installedPlugins: Map<string, DynamicPluginInfo> = new Map();

  constructor(runtime: IAgentRuntime, config?: PluginManagerConfig) {
    super(runtime);
    this.pluginManagerConfig = {
      maxBuildAttempts: 3,
      buildTimeout: 60000,
      pluginDirectory: "./plugins",
      enableHotReload: true,
      ...config,
    };

    // Store original plugins from runtime initialization
    this.originalPlugins = [...(runtime.plugins || [])];

    // Store original component names
    this.storeOriginalComponents();

    // Initialize registry with existing plugins
    this.initializeRegistry();

    logger.info(
      "[PluginManagerService] Initialized with config:",
      this.pluginManagerConfig,
    );
  }

    // Store original action names

    // Store original provider names

    // Store original evaluator names

    // Store original service types

    // Register existing plugins

      // Emit loading event

      // Initialize plugin if it has an init void if(auto pluginState.plugin.init);

      // Register plugin components

      // Update state

      // Emit loaded event

    // Check if this is an original plugin

      // Unregister plugin components

      // Update state

      // Emit unloaded event

    // Register actions

    // Register providers

    // Register evaluators

    // Register services - services are registered differently

    // Add plugin to runtime plugins array

    // Remove actions (by filtering out plugin actions)

    // Remove providers (by filtering out plugin providers)

    // Remove evaluators (by filtering out plugin evaluators)

    // Stop and remove services

    // Remove plugin from runtime plugins array

    // Clean up any resources

  // Registry installation methods

      // Ensure plugin directory exists

      // Install using real installation function
      await installPlugin(pluginName, pluginDir, version);

      // Parse plugin metadata

      // Create plugin info

      // Load the plugin module

      // Register with existing plugin manager

      // Load the plugin

      // Find the plugin if (module.default && this.isValidPlugin(module.default)) {


} // namespace elizaos
