#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
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
std::future<void> installPlugin(const std::string& pluginName, const std::string& targetDir, std::optional<std::string> version);

// Install plugin from npm
std::future<void> installFromNpm(const std::string& packageName, const std::string& version, const std::string& targetDir);

// Install plugin from git repository
std::future<void> installFromGit(const std::string& gitRepo, const std::string& version, const std::string& targetDir);

class PluginManagerService {
public:
    PluginManagerService(IAgentRuntime runtime, std::optional<PluginManagerConfig> config);
    std::future<PluginManagerService> start(IAgentRuntime runtime, std::optional<PluginManagerConfig> config);
    void storeOriginalComponents();
    void initializeRegistry();
    PluginState getPlugin(const std::string& id);
    std::vector<PluginState> getAllPlugins();
    std::vector<PluginState> getLoadedPlugins();
    void updatePluginState(const std::string& id, const std::optional<PluginState>& update);
    std::future<void> loadPlugin(auto {
    pluginId, auto force = false, LoadPluginParams });
    std::future<void> unloadPlugin(UnloadPluginParams { pluginId });
    std::future<std::string> registerPlugin(Plugin plugin);
    std::future<void> registerPluginComponents(Plugin plugin);
    std::future<void> unregisterPluginComponents(Plugin plugin);
    std::future<void> stop();
    std::future<DynamicPluginInfo> installPluginFromRegistry(const std::string& pluginName, std::optional<std::string> version);
    std::future<std::string> loadInstalledPlugin(const std::string& pluginName);
    DynamicPluginInfo getInstalledPluginInfo(const std::string& pluginName);
    std::vector<DynamicPluginInfo> listInstalledPlugins();
    std::string getPluginInstallPath(const std::string& pluginName);
    void if(auto !packageJson);


} // namespace elizaos
