#pragma once
#include <any>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".coreExtensions.hpp"
#include ".types.hpp"
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Registry installation types and functions
struct RegistryEntry {
    std::string name;
    std::optional<std::string> description;
    std::string repository;
    std::string repo;
    std::optional<std::string> v1;
    std::string repo;
    std::optional<std::string> branch;
    std::optional<std::string> version;
};

struct DynamicPluginInfo {
    std::string name;
    std::string version;
    std::string status;
    std::string path;
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

    // Install from git

// Install plugin from npm

  // Install the package to the target directory

// Install plugin from git repository

  // Clone the repository to a temporary directory

    // Clone the repository

    // Checkout specific version/branch if specified

    // Install dependencies

    // Copy to target directory
    // Clean up temp directory

class PluginManagerService {
public:
    PluginManagerService(IAgentRuntime runtime, std::optional<PluginManagerConfig> config);
    static std::future<PluginManagerService> start(IAgentRuntime runtime, std::optional<PluginManagerConfig> config);
    void storeOriginalComponents();
    void initializeRegistry();
    PluginState getPlugin(const std::string& id);
    std::vector<PluginState> getAllPlugins();
    std::vector<PluginState> getLoadedPlugins();
    void updatePluginState(const std::string& id, const std::optional<PluginState>& update);
    std::future<void> loadPlugin(auto force);
    std::future<void> unloadPlugin();
    std::future<std::string> registerPlugin(ElizaPlugin plugin);
    void trackComponentRegistration(const std::string& pluginId, const std::string& componentName);
    std::future<void> registerPluginComponents(ElizaPlugin plugin);
    std::future<void> unregisterPluginComponents(ElizaPlugin plugin);
    PluginComponents getPluginComponents(const std::string& pluginId);
    std::vector<ComponentRegistration> getComponentRegistrations(const std::string& pluginId);
    std::future<void> stop();
    bool isProtectedPlugin(const std::string& pluginName);
    std::vector<std::string> getProtectedPlugins();
    std::vector<std::string> getOriginalPlugins();
    bool canUnloadPlugin(const std::string& pluginName);
    std::string getProtectionReason(const std::string& pluginName);
    std::future<std::string> loadInstalledPlugin(const std::string& pluginName);
    std::future<std::unordered_map<std::string, RegistryEntry>> getAvailablePluginsFromRegistry();
    DynamicPluginInfo getInstalledPluginInfo(const std::string& pluginName);
    std::vector<DynamicPluginInfo> listInstalledPlugins();
    std::string getPluginInstallPath(const std::string& pluginName);
    Promise< parsePluginMetadata(const std::string& pluginPath);
    std::future<ElizaPlugin> loadPluginModule(const std::string& pluginPath);
    obj is ElizaPlugin isValidPlugin(const std::any& obj);

private:
    PluginManagerConfig pluginManagerConfig_;
};


} // namespace elizaos
