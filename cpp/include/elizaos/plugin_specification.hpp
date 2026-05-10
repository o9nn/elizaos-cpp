#pragma once

/**
 * ElizaOS C++ - PluginSpecification Module
 *
 * Plugin contract: manifest, interface, and registry.
 */

#include "elizaos.hpp"
#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace elizaos {

// ============================================================================
// PluginManifest
// ============================================================================

struct PluginManifest {
    std::string              name;
    std::string              version;
    std::string              author;
    std::string              description;
    std::vector<std::string> dependencies;
    std::vector<std::string> capabilities;
    std::string              entryPoint;
    std::unordered_map<std::string, std::string> metadata;
};

// ============================================================================
// PluginInterface
// ============================================================================

class PluginInterface {
public:
    virtual ~PluginInterface() = default;

    virtual bool initialize(const JsonValue& config) = 0;
    virtual void shutdown() = 0;
    virtual bool isInitialized() const = 0;

    virtual JsonValue execute(const std::string& action, const JsonValue& params) = 0;

    virtual const PluginManifest& getManifest() const = 0;
    virtual std::vector<std::string> getCapabilities() const = 0;
    virtual std::string getStatus() const = 0;
};

// ============================================================================
// PluginRegistry
// ============================================================================

class PluginRegistry {
public:
    PluginRegistry();

    // Registration
    bool registerPlugin(std::shared_ptr<PluginInterface> plugin);
    bool unregisterPlugin(const std::string& pluginName);

    // Lookup
    std::shared_ptr<PluginInterface>              getPlugin(const std::string& name) const;
    std::optional<PluginManifest>                 getManifest(const std::string& name) const;
    std::vector<std::shared_ptr<PluginInterface>> getAllPlugins() const;
    std::vector<std::string>                      getPluginNames() const;

    // Loading (dynamic, path-based)
    bool loadPlugin(const std::string& path);
    bool unloadPlugin(const std::string& name);

    // Capabilities
    std::vector<std::shared_ptr<PluginInterface>> findPluginsByCapability(const std::string& capability) const;

    // Lifecycle
    bool initializeAll(const JsonValue& config);
    void shutdownAll();

    void clear();
    size_t getPluginCount() const;

private:
    std::unordered_map<std::string, std::shared_ptr<PluginInterface>> plugins_;
    mutable std::mutex registryMutex_;
};

// ============================================================================
// Global registry
// ============================================================================

extern std::shared_ptr<PluginRegistry> globalPluginRegistry;

} // namespace elizaos
