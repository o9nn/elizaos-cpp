#pragma once

#include "elizaos/core.hpp"
#include "elizaos/plugins_automation.hpp"

#include <future>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace elizaos {

struct RegistryEntry {
    std::string name;
    std::string repositoryUrl;
    std::string description;
    std::string version;
    std::string author;
    std::vector<std::string> tags;
    std::map<std::string, std::string> dependencies;
    std::map<std::string, std::string> metadata;

    RegistryEntry() = default;
    RegistryEntry(const std::string& n, const std::string& repoUrl)
        : name(n), repositoryUrl(repoUrl) {}
};

struct RegistryConfig {
    std::string registryUrl;
    std::string cacheDirectory;
    int cacheTtlSeconds;
    bool enableRemoteRegistry;

    RegistryConfig()
        : registryUrl()
        , cacheDirectory("~/.elizaos/registry_cache")
        , cacheTtlSeconds(3600)
        , enableRemoteRegistry(false) {}
};

class Registry {
public:
    Registry();
    explicit Registry(const RegistryConfig& config);
    ~Registry();

    Registry(const Registry&) = delete;
    Registry& operator=(const Registry&) = delete;

    std::future<bool> refreshRegistry();
    bool loadLocalRegistry(const std::string& registryFilePath = "");
    bool registerEntry(const RegistryEntry& entry);
    bool unregisterEntry(const std::string& name);
    std::vector<RegistryEntry> getAllPlugins() const;
    std::vector<RegistryEntry> searchPlugins(const std::string& query) const;
    std::vector<RegistryEntry> getPluginsSortedByVersion() const;
    std::optional<RegistryEntry> getPlugin(const std::string& name) const;

    PluginRegistry& getPluginRegistry() noexcept { return pluginRegistry_; }
    const PluginRegistry& getPluginRegistry() const noexcept { return pluginRegistry_; }

    bool isRegistryCached() const;
    bool saveCacheSnapshot() const;
    std::string getLastRefreshTime() const;
    size_t getPluginCount() const;

    void setConfig(const RegistryConfig& config);
    bool trySetConfig(const RegistryConfig& config) noexcept;
    RegistryConfig getConfigSnapshot() const;
    const RegistryConfig& getConfig() const;

    static int compareSemanticVersions(const std::string& lhs,
                                       const std::string& rhs);

private:
    RegistryConfig config_;
    std::unordered_map<std::string, RegistryEntry> entries_;
    PluginRegistry pluginRegistry_;
    mutable std::mutex registryMutex_;
    std::string lastRefreshTime_;

    bool parseRegistryJson(const std::string& jsonContent);
    bool loadFromCache();
    bool saveToCache() const;
    std::string getCacheFilePath(const RegistryConfig& config) const;
    bool isHttpUrl(const std::string& url) const;
    std::string downloadRegistryData(const std::string& url) const;
    std::string expandPath(const std::string& path) const;
    void updateLastRefreshTime();
};

// Prefer the shared accessor whenever the instance may be retained across a
// replacement. The legacy reference is pinned for the calling thread.
std::shared_ptr<Registry> getGlobalRegistryShared();
Registry& getGlobalRegistry();
void setGlobalRegistry(std::shared_ptr<Registry> registry);
void setGlobalRegistry(std::unique_ptr<Registry> registry);

} // namespace elizaos
