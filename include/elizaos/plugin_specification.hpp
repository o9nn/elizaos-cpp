#pragma once

#include "elizaos/core.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>
#include <optional>
#include <variant>

namespace elizaos {

/**
 * Plugin specification system for ElizaOS C++
 * 
 * This module defines the interface and standards for creating plugins
 * that extend the functionality of the ElizaOS framework.
 */

// Forward declarations
class Plugin;
class PluginManager;
class PluginInterface;
class PluginMetadata;

/**
 * Plugin API version specification
 */
struct PluginAPIVersion {
    uint32_t major = 1;
    uint32_t minor = 0;
    uint32_t patch = 0;
    
    PluginAPIVersion(uint32_t maj = 1, uint32_t min = 0, uint32_t pat = 0) 
        : major(maj), minor(min), patch(pat) {}
    
    std::string toString() const {
        return std::to_string(major) + "." + std::to_string(minor) + "." + std::to_string(patch);
    }
    
    bool isCompatible(const PluginAPIVersion& other) const {
        // Simple semantic versioning compatibility
        return major == other.major && minor >= other.minor;
    }
};

/**
 * Plugin capability types
 */
enum class PluginCapability {
    ACTION_PROVIDER,       // Provides new actions
    MEMORY_PROVIDER,       // Provides memory implementations
    EVALUATOR,            // Provides content evaluation
    PROVIDER,             // Provides state composition
    CHARACTER_PROVIDER,    // Provides character definitions
    KNOWLEDGE_PROVIDER,    // Provides knowledge sources
    COMMUNICATION,        // Communication interfaces
    TOOL_INTEGRATION,     // External tool integrations
    CUSTOM                // Custom capability
};

/**
 * Plugin dependency specification
 */
struct PluginDependency {
    std::string name;
    PluginAPIVersion minVersion;
    PluginAPIVersion maxVersion;
    bool optional = false;
    std::string description;
    
    PluginDependency(const std::string& n, const PluginAPIVersion& minVer = PluginAPIVersion(),
                    const PluginAPIVersion& maxVer = PluginAPIVersion(999, 999, 999),
                    bool opt = false, const std::string& desc = "")
        : name(n), minVersion(minVer), maxVersion(maxVer), optional(opt), description(desc) {}
};

/**
 * Plugin configuration schema
 */
struct PluginConfigSchema {
    std::string name;
    std::string type; // "string", "number", "boolean", "array", "object"
    std::string description;
    bool required = false;
    std::variant<std::string, double, bool> defaultValue;
    std::vector<std::string> allowedValues; // For enum-like configs
};

/**
 * Plugin metadata containing information about the plugin
 */
class PluginMetadata {
public:
    PluginMetadata() = default;  // Default constructor
    PluginMetadata(const std::string& name, const PluginAPIVersion& version);
    
    // Basic information
    const std::string& getName() const { return name_; }
    const PluginAPIVersion& getVersion() const { return version_; }
    const std::string& getDescription() const { return description_; }
    const std::string& getAuthor() const { return author_; }
    const std::string& getLicense() const { return license_; }
    const std::string& getHomepage() const { return homepage_; }
    
    // Capabilities
    const std::vector<PluginCapability>& getCapabilities() const { return capabilities_; }
    void addCapability(PluginCapability capability);
    bool hasCapability(PluginCapability capability) const;
    
    // Dependencies
    const std::vector<PluginDependency>& getDependencies() const { return dependencies_; }
    void addDependency(const PluginDependency& dependency);
    
    // Configuration schema
    const std::vector<PluginConfigSchema>& getConfigSchema() const { return configSchema_; }
    void addConfigField(const PluginConfigSchema& field);
    
    // Setters for metadata
    void setDescription(const std::string& description) { description_ = description; }
    void setAuthor(const std::string& author) { author_ = author; }
    void setLicense(const std::string& license) { license_ = license; }
    void setHomepage(const std::string& homepage) { homepage_ = homepage; }
    
    // Validation
    bool isValid() const;
    std::vector<std::string> validate() const;

private:
    std::string name_;
    PluginAPIVersion version_;
    std::string description_;
    std::string author_;
    std::string license_;
    std::string homepage_;
    std::vector<PluginCapability> capabilities_;
    std::vector<PluginDependency> dependencies_;
    std::vector<PluginConfigSchema> configSchema_;
};

/**
 * Plugin interface that all plugins must implement
 */
class PluginInterface {
public:
    virtual ~PluginInterface() = default;
    
    // Core plugin lifecycle
    virtual bool initialize(const std::unordered_map<std::string, std::string>& config) = 0;
    virtual bool start() = 0;
    virtual bool stop() = 0;
    virtual void shutdown() = 0;
    
    // Plugin information
    virtual PluginMetadata getMetadata() const = 0;
    virtual std::string getStatus() const = 0;
    virtual bool isHealthy() const = 0;
    
    // Configuration management
    virtual std::vector<PluginConfigSchema> getConfigSchema() const = 0;
    virtual bool validateConfig(const std::unordered_map<std::string, std::string>& config) const = 0;
    virtual std::unordered_map<std::string, std::string> getDefaultConfig() const = 0;
};

/**
 * Plugin loading and validation results
 */
struct PluginLoadResult {
    bool success = false;
    std::string errorMessage;
    std::shared_ptr<PluginInterface> plugin;
    PluginMetadata metadata;
    
    PluginLoadResult() = default;
    PluginLoadResult(bool success, const std::string& error = "")
        : success(success), errorMessage(error) {}
};

/**
 * Plugin discovery information
 */
struct PluginInfo {
    std::string name;
    std::string path;
    PluginMetadata metadata;
    bool isLoaded = false;
    bool isEnabled = false;
    std::string status;
};

/**
 * Plugin manager for loading, managing, and coordinating plugins
 */
class PluginManager {
public:
    PluginManager();
    ~PluginManager();
    
    // Plugin discovery
    std::vector<PluginInfo> discoverPlugins(const std::string& directory);
    std::vector<PluginInfo> getAvailablePlugins() const;
    
    // Plugin lifecycle management
    PluginLoadResult loadPlugin(const std::string& path, 
                               const std::unordered_map<std::string, std::string>& config = {});
    bool unloadPlugin(const std::string& name);
    bool enablePlugin(const std::string& name);
    bool disablePlugin(const std::string& name);
    
    // Plugin queries
    std::shared_ptr<PluginInterface> getPlugin(const std::string& name) const;
    std::vector<std::shared_ptr<PluginInterface>> getPluginsByCapability(PluginCapability capability) const;
    std::vector<std::string> getLoadedPluginNames() const;
    
    // Configuration management
    bool configurePlugin(const std::string& name, 
                        const std::unordered_map<std::string, std::string>& config);
    std::unordered_map<std::string, std::string> getPluginConfig(const std::string& name) const;
    
    // Validation and dependency resolution
    bool validateDependencies(const PluginMetadata& metadata) const;
    std::vector<std::string> resolveDependencyOrder(const std::vector<std::string>& pluginNames) const;
    
    // Plugin health monitoring
    std::vector<std::string> getUnhealthyPlugins() const;
    bool performHealthCheck(const std::string& name) const;
    void performAllHealthChecks();
    
    // Events and callbacks
    using PluginEventCallback = std::function<void(const std::string& pluginName, const std::string& event)>;
    void setEventCallback(PluginEventCallback callback) { eventCallback_ = callback; }

private:
    std::unordered_map<std::string, std::shared_ptr<PluginInterface>> loadedPlugins_;
    std::unordered_map<std::string, PluginInfo> availablePlugins_;
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> pluginConfigs_;
    PluginEventCallback eventCallback_;
    
    // Internal helper methods
    bool validatePluginPath(const std::string& path) const;
    PluginMetadata loadPluginMetadata(const std::string& path) const;
    void notifyEvent(const std::string& pluginName, const std::string& event);
    std::vector<std::string> findCircularDependencies(const std::vector<std::string>& pluginNames) const;
};

/**
 * Plugin registry for registering plugin factories
 */
class PluginRegistry {
public:
    using PluginFactory = std::function<std::shared_ptr<PluginInterface>()>;
    
    static PluginRegistry& getInstance();
    
    void registerPlugin(const std::string& name, PluginFactory factory, const PluginMetadata& metadata);
    void unregisterPlugin(const std::string& name);
    
    std::shared_ptr<PluginInterface> createPlugin(const std::string& name) const;
    std::vector<std::string> getRegisteredPlugins() const;
    PluginMetadata getPluginMetadata(const std::string& name) const;

private:
    PluginRegistry() = default;
    std::unordered_map<std::string, PluginFactory> factories_;
    std::unordered_map<std::string, PluginMetadata> metadata_;
};

/**
 * Macro for easy plugin registration
 */
#define REGISTER_PLUGIN(PluginClass, name, version) \
    namespace { \
        struct PluginRegistrator_##PluginClass { \
            PluginRegistrator_##PluginClass() { \
                elizaos::PluginRegistry::getInstance().registerPlugin( \
                    name, \
                    []() -> std::shared_ptr<elizaos::PluginInterface> { \
                        return std::make_shared<PluginClass>(); \
                    }, \
                    PluginClass::createMetadata(name, version) \
                ); \
            } \
        }; \
        static PluginRegistrator_##PluginClass pluginRegistrator_##PluginClass; \
    }

/**
 * Plugin utility functions
 */
namespace plugin_utils {
    std::string capabilityToString(PluginCapability capability);
    PluginCapability stringToCapability(const std::string& str);
    
    bool isValidPluginName(const std::string& name);
    bool isValidVersion(const std::string& version);
    
    std::string generatePluginID(const PluginMetadata& metadata);
    std::vector<std::string> parsePluginPath(const std::string& path);
}

} // namespace elizaos