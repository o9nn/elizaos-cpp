#include ".coreExtensions.hpp"
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
    'installed' | 'loaded' | 'active' | 'inactive' | 'error' | 'needs_configuration' status;
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

  // Ensure target directory exists

  // Get registry entry to determine installation method

  // Determine installation method
    // Install from npm

    await installFromNpm(packageName, packageVersion, targetDir, onProgress);
    // Install from git

    await installFromGit(gitRepo, gitVersion, targetDir, onProgress);

// Install plugin from npm

  // Install the package to the target directory

// Install plugin from git repository

  // Clone the repository to a temporary directory

    // Clone the repository

    // Checkout specific version/branch if specified

    // Install dependencies

    // Copy to target directory
    // Clean up temp directory

class PluginManagerService extends Service implements PluginRegistry {
  static override serviceType: ServiceTypeName = PluginManagerServiceType.PLUGIN_MANAGER;
  override capabilityDescription =
    'Manages dynamic loading and unloading of plugins at runtime, including registry installation';

  public plugins: Map<string, PluginState> = new Map();
  private pluginManagerConfig: PluginManagerConfig;
  private originalPlugins: ElizaPlugin[] = [];
  private originalActions: Set<string> = new Set();
  private originalProviders: Set<string> = new Set();
  private originalEvaluators: Set<string> = new Set();
  private originalServices: Set<string> = new Set();

  // Add registry installation state management
  private installedPlugins: Map<string, DynamicPluginInfo> = new Map();

  // Component tracking
  private componentRegistry: Map<string, ComponentRegistration[]> = new Map();

  // Protected plugins that cannot be registered, loaded, or unloaded by external code
  // These match the actual plugin names as defined in their respective index.ts files
  private readonly PROTECTED_PLUGINS = new Set<string>([
    'plugin-manager', // The plugin manager itself
    '@elizaos/plugin-sql', // SQL database plugin
    'bootstrap', // Bootstrap plugin
    'game-api', // Game API plugin
    'inference', // Inference engine
    'autonomy', // Autonomy plugin
    'knowledge', // Knowledge management
    '@elizaos/plugin-personality', // Personality system
    'experience', // Experience tracking
    'goals', // Goals tracking (can be removed once progression is working)
    'todo', // Todo tracking (can be removed once progression is working)
  ]);

  constructor(runtime: IAgentRuntime, config?: PluginManagerConfig) {
    super(runtime);
    this.pluginManagerConfig = {
      maxBuildAttempts: 3,
      buildTimeout: 60000,
      pluginDirectory: './plugins',
      enableHotReload: true,
      ...config,
    };

    // Apply runtime extensions for plugin management
    applyRuntimeExtensions(runtime);

    // Store original plugins from runtime initialization
    this.originalPlugins = [...(runtime.plugins || [])];

    // Store original component names
    this.storeOriginalComponents();

    // Initialize registry with existing plugins
    this.initializeRegistry();

    logger.info('[PluginManagerService] Initialized with config:', this.pluginManagerConfig);
  }

    // Store original action names

    // Store original provider names

    // Store original evaluator names

    // Store original service types

    // Register existing plugins

      // Track original plugin components

    // Don't allow force loading of protected plugins from external code

    // Initialize plugin if it has an init void if(auto pluginState.plugin.init);

    // Register plugin components

    // Update state

    // Check if this is an original plugin

    // Check if this is a protected plugin

    // Unregister plugin components

    // Update state

    // Check if trying to register a duplicate of an original plugin

    // Check if this is an attempt to register a protected plugin

    // Register actions

    // Register providers

    // Register evaluators

    // Register event handlers and track them

    // Register services - services are registered differently

        // Get existing services array or create new one

        // Store the updated array

    // Add plugin to runtime plugins array

    // Remove actions (by filtering out plugin actions)

    // Remove providers (by filtering out plugin providers)

    // Remove evaluators (by filtering out plugin evaluators)

    // Unregister event handlers

    // Stop and remove services
            // Stop each service in the array

    // Remove plugin from runtime plugins array

    // Clear component registry for this plugin

  // Helper method to get plugin components

  // Helper method to get component registrations

    // Clean up any resources

  /**
   * Checks if a plugin name is protected and cannot be modified
   * Also checks for name variations (with/without @elizaos/ prefix)
   */
    // Check exact match

    // Check without @elizaos/ prefix

    // Check with @elizaos/ prefix added

    // Also protect original plugins (loaded at startup)

  /**
   * Gets the list of protected plugin names
   */

  /**
   * Gets the list of original plugin names (loaded at startup)
   */

  /**
   * Checks if a plugin can be safely unloaded
   * Returns true if the plugin can be unloaded, false if it's protected
   */

  /**
   * Gets a human-readable reason why a plugin cannot be unloaded
   */

  // Registry installation methods

    // Ensure plugin directory exists

    // Install using real installation function
    await installPlugin(pluginName, pluginDir, version, onProgress);

    // Parse plugin metadata

    // Create plugin info

    // Validate that the path exists

    // Check if it's a directory or a tarball

      // Handle compressed bundles (tar.gz, zip)

    // Parse plugin metadata

    // Copy to installation directory

    // Install dependencies if needed

    // Create plugin info

    // Load the plugin module

    // Register with existing plugin manager

    // Load the plugin

    // Find the plugin if (module.default && this.isValidPlugin(module.default)) {


} // namespace elizaos
