#pragma once

#include "elizaos/core.hpp"

#include <any>
#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

namespace elizaos {

class PluginManager;
class PluginContext;

struct PluginVersion {
    int major = 1;
    int minor = 0;
    int patch = 0;

    std::string toString() const;
    bool isValid() const;
    bool isCompatibleWith(const PluginVersion& other) const;
    static std::optional<PluginVersion> tryParse(const std::string& versionStr);
    static PluginVersion parse(const std::string& versionStr);
};

struct PluginMetadata {
    std::string id;
    std::string name;
    std::string description;
    std::string author;
    PluginVersion version;
    std::vector<std::string> tags;
    std::vector<std::string> dependencies;
};

enum class PluginState {
    UNLOADED,
    LOADED,
    INITIALIZED,
    ACTIVE,
    SUSPENDED,
    ERROR
};

class PluginConfig {
public:
    PluginConfig() = default;
    PluginConfig(const PluginConfig& other);
    PluginConfig& operator=(const PluginConfig& other);
    PluginConfig(PluginConfig&& other);
    PluginConfig& operator=(PluginConfig&& other);

    void set(const std::string& key, const std::string& value);
    void set(const std::string& key, const char* value);
    void set(const std::string& key, int value);
    void set(const std::string& key, double value);
    void set(const std::string& key, bool value);

    std::string getString(const std::string& key, const std::string& defaultValue = "") const;
    int getInt(const std::string& key, int defaultValue = 0) const;
    double getDouble(const std::string& key, double defaultValue = 0.0) const;
    bool getBool(const std::string& key, bool defaultValue = false) const;
    bool has(const std::string& key) const;
    bool validate(std::string& errorMessage) const;
    void addRequiredKey(const std::string& key);

private:
    mutable std::mutex mutex_;
    std::unordered_map<std::string, std::any> values_;
    std::vector<std::string> requiredKeys_;
};

class PluginContext {
public:
    explicit PluginContext(const std::string& pluginId);

    const std::string& getPluginId() const { return pluginId_; }
    void setAgentState(std::shared_ptr<State> state);
    std::shared_ptr<State> getAgentState() const;

    void log(const std::string& message) const;
    void logWarning(const std::string& message) const;
    void logError(const std::string& message) const;

    void setData(const std::string& key, std::any value);
    std::any getData(const std::string& key) const;
    bool hasData(const std::string& key) const;

private:
    std::string pluginId_;
    mutable std::mutex mutex_;
    std::shared_ptr<State> agentState_;
    std::unordered_map<std::string, std::shared_ptr<const std::any>> pluginData_;
};

class IPlugin {
public:
    virtual ~IPlugin() = default;
    virtual bool initialize(const PluginConfig& config, PluginContext& context) = 0;
    virtual void shutdown() = 0;
    virtual PluginMetadata getMetadata() const = 0;
    virtual PluginState getState() const = 0;
    virtual void setState(PluginState state) = 0;
    virtual bool healthCheck() const { return getState() == PluginState::ACTIVE; }
};

class IActionPlugin : public IPlugin {
public:
    ~IActionPlugin() override = default;
    virtual bool validate(const Memory& message, const State& state) const = 0;
    virtual bool execute(const Memory& message, State& state) = 0;
    virtual std::vector<std::string> getExamples() const { return {}; }
    virtual std::string getActionDescription() const = 0;
};

class IEvaluatorPlugin : public IPlugin {
public:
    ~IEvaluatorPlugin() override = default;
    virtual double evaluate(const Memory& memory, const State& state) const = 0;
    virtual std::string getEvaluationCriteria() const = 0;
};

class IProviderPlugin : public IPlugin {
public:
    ~IProviderPlugin() override = default;
    virtual std::string provide(const State& state, const Memory* message = nullptr) = 0;
    virtual bool isReady() const = 0;
};

using PluginFactory = std::function<std::unique_ptr<IPlugin>()>;

class PluginRegistry {
public:
    static PluginRegistry& instance();

    void registerPlugin(const std::string& pluginId, PluginFactory factory);
    bool tryRegisterPlugin(const std::string& pluginId, PluginFactory factory);
    void unregisterPlugin(const std::string& pluginId);
    std::unique_ptr<IPlugin> createPlugin(const std::string& pluginId) const;
    bool hasPlugin(const std::string& pluginId) const;
    std::vector<std::string> getRegisteredPluginIds() const;
    void clear();

private:
    PluginRegistry() = default;
    mutable std::mutex mutex_;
    std::unordered_map<std::string, std::shared_ptr<const PluginFactory>> factories_;
};

class PluginManager {
private:
    // The context is declared before the plugin so destruction occurs in the
    // reverse (plugin-then-context) order. Aliasing shared_ptr accessors keep
    // this complete ownership record alive.
    struct ManagedPlugin {
        std::shared_ptr<PluginContext> context;
        std::shared_ptr<IPlugin> plugin;
        std::vector<std::string> dependencies;
    };

public:
    PluginManager();
    ~PluginManager();

    PluginManager(const PluginManager&) = delete;
    PluginManager& operator=(const PluginManager&) = delete;

    bool loadPlugin(const std::string& pluginId, const PluginConfig& config = PluginConfig{});
    bool unloadPlugin(const std::string& pluginId);
    void unloadAll();

    // Legacy borrowed accessors. Loaded instances are retained until manager
    // destruction, so an unload cannot dangle a pointer already returned by
    // this manager. The plugin may be shut down; use shared access when the
    // handle must also outlive the manager.
    IPlugin* getPlugin(const std::string& pluginId);
    const IPlugin* getPlugin(const std::string& pluginId) const;
    std::shared_ptr<IPlugin> getPluginShared(const std::string& pluginId) const;

    template<typename T>
    std::shared_ptr<T> getPluginAsShared(const std::string& pluginId) const {
        return std::dynamic_pointer_cast<T>(getPluginShared(pluginId));
    }

    template<typename T>
    T* getPluginAs(const std::string& pluginId) {
        return dynamic_cast<T*>(getPlugin(pluginId));
    }

    template<typename T>
    std::vector<std::shared_ptr<T>> getPluginsOfTypeShared() const {
        std::vector<std::shared_ptr<IPlugin>> snapshot;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            snapshot.reserve(plugins_.size());
            for (const auto& entry : plugins_) {
                snapshot.emplace_back(entry.second, entry.second->plugin.get());
            }
        }
        std::vector<std::shared_ptr<T>> result;
        for (const auto& plugin : snapshot) {
            if (auto typed = std::dynamic_pointer_cast<T>(plugin)) result.push_back(std::move(typed));
        }
        return result;
    }

    template<typename T>
    std::vector<T*> getPluginsOfType() {
        std::vector<T*> result;
        for (const auto& plugin : getPluginsOfTypeShared<T>()) result.push_back(plugin.get());
        return result;
    }

    bool suspendPlugin(const std::string& pluginId);
    bool resumePlugin(const std::string& pluginId);
    std::vector<std::string> getLoadedPluginIds() const;
    bool checkPluginHealth(const std::string& pluginId) const;
    void setAgentState(std::shared_ptr<State> state);

private:
    bool unloadPluginInternal(const std::string& pluginId, bool duringUnloadAll);
    void unloadAllInternal(bool duringDestruction);
    bool hasDependentsLocked(const std::string& pluginId) const;
    bool currentThreadOwnsTransitionLocked() const;
    void finishTransitionLocked(const std::string& pluginId);

    mutable std::mutex mutex_;
    std::condition_variable transitionCondition_;
    std::unordered_map<std::string, std::shared_ptr<ManagedPlugin>> plugins_;
    std::vector<std::shared_ptr<ManagedPlugin>> retiredPlugins_;
    std::unordered_map<std::string, std::vector<std::string>> dependencies_;
    std::unordered_map<std::string, std::thread::id> transitioning_;
    std::shared_ptr<State> agentState_;
    bool unloadingAll_ = false;
    bool shuttingDown_ = false;
    std::thread::id unloadingThread_;
};

#define ELIZAOS_PLUGIN_STARTER_CONCAT_INNER(left, right) left##right
#define ELIZAOS_PLUGIN_STARTER_CONCAT(left, right) \
    ELIZAOS_PLUGIN_STARTER_CONCAT_INNER(left, right)
#define REGISTER_ELIZA_PLUGIN(PluginClass, pluginId) \
    namespace { \
        [[maybe_unused]] const bool ELIZAOS_PLUGIN_STARTER_CONCAT( \
            elizaos_plugin_registered_, __COUNTER__) = []() { \
            return elizaos::PluginRegistry::instance().tryRegisterPlugin( \
                pluginId, \
                []() -> std::unique_ptr<elizaos::IPlugin> { \
                    return std::make_unique<PluginClass>(); \
                }); \
        }(); \
    }

class SimpleActionPlugin : public IActionPlugin {
public:
    SimpleActionPlugin(const std::string& id, const std::string& name,
                       const std::string& description);
    ~SimpleActionPlugin() override = default;

    bool initialize(const PluginConfig& config, PluginContext& context) override;
    void shutdown() override;
    PluginMetadata getMetadata() const override;
    PluginState getState() const override { return state_.load(); }
    void setState(PluginState state) override { state_.store(state); }

protected:
    PluginMetadata metadata_;
    std::atomic<PluginState> state_{PluginState::UNLOADED};
    PluginContext* context_ = nullptr;
    PluginConfig config_;
};

class SimpleProviderPlugin : public IProviderPlugin {
public:
    SimpleProviderPlugin(const std::string& id, const std::string& name,
                         const std::string& description);
    ~SimpleProviderPlugin() override = default;

    bool initialize(const PluginConfig& config, PluginContext& context) override;
    void shutdown() override;
    PluginMetadata getMetadata() const override;
    PluginState getState() const override { return state_.load(); }
    void setState(PluginState state) override { state_.store(state); }
    bool isReady() const override { return state_.load() == PluginState::ACTIVE; }

protected:
    PluginMetadata metadata_;
    std::atomic<PluginState> state_{PluginState::UNLOADED};
    PluginContext* context_ = nullptr;
    PluginConfig config_;
};

namespace PluginUtils {
    std::string generatePluginId(const std::string& prefix = "plugin");
    bool validateMetadata(const PluginMetadata& metadata, std::string& error);
    bool checkDependencies(const std::vector<std::string>& deps,
                           const PluginManager& manager,
                           std::vector<std::string>& missing);
    std::string stateToString(PluginState state);
    std::optional<PluginState> tryStateFromString(const std::string& str);
    PluginState stateFromString(const std::string& str);
}

} // namespace elizaos
