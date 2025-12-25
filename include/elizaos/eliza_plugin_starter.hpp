#pragma once

#include "elizaos/core.hpp"
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>
#include <any>

namespace elizaos {

/**
 * ElizaOS Plugin Development Starter
 *
 * This module provides a comprehensive framework for developing plugins
 * for the ElizaOS agent system. It includes:
 * - Plugin interface definitions
 * - Plugin lifecycle management
 * - Plugin registration and discovery
 * - Action, evaluator, and provider plugin types
 * - Plugin configuration and validation
 */

// Forward declarations
class PluginManager;
class PluginContext;

/**
 * Plugin version information
 */
struct PluginVersion {
    int major = 1;
    int minor = 0;
    int patch = 0;

    std::string toString() const;
    bool isCompatibleWith(const PluginVersion& other) const;
    static PluginVersion parse(const std::string& versionStr);
};

/**
 * Plugin metadata for discovery and management
 */
struct PluginMetadata {
    std::string id;                      // Unique plugin identifier
    std::string name;                    // Human-readable name
    std::string description;             // Plugin description
    std::string author;                  // Plugin author
    PluginVersion version;               // Plugin version
    std::vector<std::string> tags;       // Searchable tags
    std::vector<std::string> dependencies;  // Required plugin IDs
};

/**
 * Plugin lifecycle states
 */
enum class PluginState {
    UNLOADED,      // Plugin not yet loaded
    LOADED,        // Plugin loaded but not initialized
    INITIALIZED,   // Plugin initialized and ready
    ACTIVE,        // Plugin actively running
    SUSPENDED,     // Plugin temporarily suspended
    ERROR          // Plugin in error state
};

/**
 * Plugin configuration with validation
 */
class PluginConfig {
public:
    PluginConfig() = default;

    // Set configuration values
    void set(const std::string& key, const std::string& value);
    void set(const std::string& key, int value);
    void set(const std::string& key, double value);
    void set(const std::string& key, bool value);

    // Get configuration values with defaults
    std::string getString(const std::string& key, const std::string& defaultValue = "") const;
    int getInt(const std::string& key, int defaultValue = 0) const;
    double getDouble(const std::string& key, double defaultValue = 0.0) const;
    bool getBool(const std::string& key, bool defaultValue = false) const;

    // Check if key exists
    bool has(const std::string& key) const;

    // Validation
    bool validate(std::string& errorMessage) const;

    // Add required key for validation
    void addRequiredKey(const std::string& key);

private:
    std::unordered_map<std::string, std::any> values_;
    std::vector<std::string> requiredKeys_;
};

/**
 * Plugin execution context
 * Provides plugins with access to the agent runtime
 */
class PluginContext {
public:
    explicit PluginContext(const std::string& pluginId);

    // Get plugin ID
    const std::string& getPluginId() const { return pluginId_; }

    // Access agent state (if available)
    void setAgentState(std::shared_ptr<State> state) { agentState_ = state; }
    std::shared_ptr<State> getAgentState() const { return agentState_; }

    // Logging within plugin context
    void log(const std::string& message) const;
    void logWarning(const std::string& message) const;
    void logError(const std::string& message) const;

    // Store plugin-specific data
    void setData(const std::string& key, std::any value);
    std::any getData(const std::string& key) const;
    bool hasData(const std::string& key) const;

private:
    std::string pluginId_;
    std::shared_ptr<State> agentState_;
    std::unordered_map<std::string, std::any> pluginData_;
};

/**
 * Base interface for all ElizaOS plugins
 */
class IPlugin {
public:
    virtual ~IPlugin() = default;

    // Plugin lifecycle
    virtual bool initialize(const PluginConfig& config, PluginContext& context) = 0;
    virtual void shutdown() = 0;

    // Plugin metadata
    virtual PluginMetadata getMetadata() const = 0;

    // State management
    virtual PluginState getState() const = 0;
    virtual void setState(PluginState state) = 0;

    // Optional: Health check
    virtual bool healthCheck() const { return getState() == PluginState::ACTIVE; }
};

/**
 * Action Plugin Interface
 * Plugins that perform actions in response to agent decisions
 */
class IActionPlugin : public IPlugin {
public:
    virtual ~IActionPlugin() = default;

    // Validate if this action can handle the given input
    virtual bool validate(const Memory& message, const State& state) const = 0;

    // Execute the action
    virtual bool execute(const Memory& message, State& state) = 0;

    // Get example prompts for this action
    virtual std::vector<std::string> getExamples() const { return {}; }

    // Get action description for LLM
    virtual std::string getActionDescription() const = 0;
};

/**
 * Evaluator Plugin Interface
 * Plugins that evaluate agent responses and memories
 */
class IEvaluatorPlugin : public IPlugin {
public:
    virtual ~IEvaluatorPlugin() = default;

    // Evaluate a memory or response
    virtual double evaluate(const Memory& memory, const State& state) const = 0;

    // Get evaluation criteria
    virtual std::string getEvaluationCriteria() const = 0;
};

/**
 * Provider Plugin Interface
 * Plugins that provide data or services to the agent
 */
class IProviderPlugin : public IPlugin {
public:
    virtual ~IProviderPlugin() = default;

    // Get data from this provider
    virtual std::string provide(const State& state, const Memory* message = nullptr) = 0;

    // Check if provider is ready
    virtual bool isReady() const = 0;
};

/**
 * Plugin Factory for creating plugin instances
 */
using PluginFactory = std::function<std::unique_ptr<IPlugin>()>;

/**
 * Plugin Registry for plugin discovery and management
 */
class PluginRegistry {
public:
    static PluginRegistry& instance();

    // Register a plugin factory
    void registerPlugin(const std::string& pluginId, PluginFactory factory);

    // Unregister a plugin
    void unregisterPlugin(const std::string& pluginId);

    // Create a plugin instance
    std::unique_ptr<IPlugin> createPlugin(const std::string& pluginId) const;

    // Check if plugin is registered
    bool hasPlugin(const std::string& pluginId) const;

    // Get all registered plugin IDs
    std::vector<std::string> getRegisteredPluginIds() const;

    // Clear all registrations
    void clear();

private:
    PluginRegistry() = default;
    std::unordered_map<std::string, PluginFactory> factories_;
};

/**
 * Plugin Manager for lifecycle management
 */
class PluginManager {
public:
    PluginManager();
    ~PluginManager();

    // Load and initialize a plugin
    bool loadPlugin(const std::string& pluginId, const PluginConfig& config = PluginConfig{});

    // Unload a plugin
    bool unloadPlugin(const std::string& pluginId);

    // Get a loaded plugin
    IPlugin* getPlugin(const std::string& pluginId);
    const IPlugin* getPlugin(const std::string& pluginId) const;

    // Get plugin by type
    template<typename T>
    T* getPluginAs(const std::string& pluginId) {
        auto* plugin = getPlugin(pluginId);
        return plugin ? dynamic_cast<T*>(plugin) : nullptr;
    }

    // Get all plugins of a specific type
    template<typename T>
    std::vector<T*> getPluginsOfType() {
        std::vector<T*> result;
        for (auto& pair : plugins_) {
            if (auto* typed = dynamic_cast<T*>(pair.second.get())) {
                result.push_back(typed);
            }
        }
        return result;
    }

    // Suspend/resume plugins
    bool suspendPlugin(const std::string& pluginId);
    bool resumePlugin(const std::string& pluginId);

    // Get all loaded plugin IDs
    std::vector<std::string> getLoadedPluginIds() const;

    // Check plugin health
    bool checkPluginHealth(const std::string& pluginId) const;

    // Set agent state for all plugins
    void setAgentState(std::shared_ptr<State> state);

private:
    std::unordered_map<std::string, std::unique_ptr<IPlugin>> plugins_;
    std::unordered_map<std::string, std::unique_ptr<PluginContext>> contexts_;
    std::shared_ptr<State> agentState_;
};

/**
 * Helper macro for plugin registration
 * Usage: REGISTER_ELIZA_PLUGIN(MyPlugin, "my-plugin-id")
 */
#define REGISTER_ELIZA_PLUGIN(PluginClass, pluginId) \
    namespace { \
        static bool _registered_##PluginClass = []() { \
            elizaos::PluginRegistry::instance().registerPlugin( \
                pluginId, \
                []() -> std::unique_ptr<elizaos::IPlugin> { \
                    return std::make_unique<PluginClass>(); \
                }); \
            return true; \
        }(); \
    }

/**
 * Base class for simple action plugins
 * Provides common functionality and reduces boilerplate
 */
class SimpleActionPlugin : public IActionPlugin {
public:
    SimpleActionPlugin(const std::string& id, const std::string& name,
                       const std::string& description);
    virtual ~SimpleActionPlugin() = default;

    // IPlugin interface
    bool initialize(const PluginConfig& config, PluginContext& context) override;
    void shutdown() override;
    PluginMetadata getMetadata() const override;
    PluginState getState() const override { return state_; }
    void setState(PluginState state) override { state_ = state; }

protected:
    PluginMetadata metadata_;
    PluginState state_ = PluginState::UNLOADED;
    PluginContext* context_ = nullptr;
};

/**
 * Base class for simple provider plugins
 */
class SimpleProviderPlugin : public IProviderPlugin {
public:
    SimpleProviderPlugin(const std::string& id, const std::string& name,
                         const std::string& description);
    virtual ~SimpleProviderPlugin() = default;

    // IPlugin interface
    bool initialize(const PluginConfig& config, PluginContext& context) override;
    void shutdown() override;
    PluginMetadata getMetadata() const override;
    PluginState getState() const override { return state_; }
    void setState(PluginState state) override { state_ = state; }

    // IProviderPlugin interface
    bool isReady() const override { return state_ == PluginState::ACTIVE; }

protected:
    PluginMetadata metadata_;
    PluginState state_ = PluginState::UNLOADED;
    PluginContext* context_ = nullptr;
};

/**
 * Utility functions for plugin development
 */
namespace PluginUtils {
    // Generate a unique plugin ID
    std::string generatePluginId(const std::string& prefix = "plugin");

    // Validate plugin metadata
    bool validateMetadata(const PluginMetadata& metadata, std::string& error);

    // Check if dependencies are satisfied
    bool checkDependencies(const std::vector<std::string>& deps,
                           const PluginManager& manager,
                           std::vector<std::string>& missing);

    // Convert plugin state to string
    std::string stateToString(PluginState state);

    // Parse plugin state from string
    PluginState stateFromString(const std::string& str);
}

} // namespace elizaos
