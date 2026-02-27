#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_PLUGIN_PLUGIN_MANAGER_SRC_TYPES_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_PLUGIN_PLUGIN_MANAGER_SRC_TYPES_H
#include "core.hpp"
// External dependency removed
using ElizaPlugin = Plugin;

namespace @elizaos/core {
    class ServiceTypeRegistry;
}
enum struct PluginStatus;
class PluginEnvironmentVariable;
class PluginComponents;
class ComponentRegistration;
class PluginState;
class PluginRegistry;
class LoadPluginParams;
class UnloadPluginParams;
class PluginManagerConfig;
class InstallProgress;
class PluginMetadata;

namespace std:("@elizaos/core") {
    class ServiceTypeRegistry : public object, public std::enable_shared_from_this<ServiceTypeRegistry> {
    public:
        using std::enable_shared_from_this<ServiceTypeRegistry>::shared_from_this;
        string PLUGIN_MANAGER;

        string PLUGIN_CONFIGURATION;

        string REGISTRY;
    };

}
extern object PluginManagerServiceType;
enum struct PluginStatus {
    BUILDING = std:("building"), READY = std:("ready"), LOADED = std:("loaded"), ERROR = std:("error"), UNLOADED = std:("unloaded"), NEEDS_CONFIGURATION = std:("needs_configuration")
};
class PluginEnvironmentVariable : public object, public std::enable_shared_from_this<PluginEnvironmentVariable> {
public:
    using std::enable_shared_from_this<PluginEnvironmentVariable>::shared_from_this;
    string name;

    string description;

    boolean sensitive;

    boolean required;

    string defaultValue;

    object validation;
};

class PluginComponents : public object, public std::enable_shared_from_this<PluginComponents> {
public:
    using std::enable_shared_from_this<PluginComponents>::shared_from_this;
    std::shared_ptr<Set<string>> actions;

    std::shared_ptr<Set<string>> providers;

    std::shared_ptr<Set<string>> evaluators;

    std::shared_ptr<Set<string>> services;

    std::shared_ptr<Map<string, std::shared_ptr<Set<std::function<std::shared_ptr<Promise<void>>(any)>>>>> eventHandlers;
};

class ComponentRegistration : public object, public std::enable_shared_from_this<ComponentRegistration> {
public:
    using std::enable_shared_from_this<ComponentRegistration>::shared_from_this;
    string pluginId;

    any componentType;

    string componentName;

    double timestamp;
};

class PluginState : public object, public std::enable_shared_from_this<PluginState> {
public:
    using std::enable_shared_from_this<PluginState>::shared_from_this;
    string id;

    string name;

    PluginStatus status;

    std::shared_ptr<ElizaPlugin> plugin;

    array<string> missingEnvVars;

    array<string> buildLog;

    string sourceCode;

    any packageJson;

    string error;

    double createdAt;

    double loadedAt;

    double unloadedAt;

    string version;

    Record<string, string> dependencies;

    any configurationStatus;

    array<std::shared_ptr<PluginEnvironmentVariable>> requiredConfiguration;

    array<string> configurationErrors;

    std::shared_ptr<PluginComponents> components;
};

class PluginRegistry : public object, public std::enable_shared_from_this<PluginRegistry> {
public:
    using std::enable_shared_from_this<PluginRegistry>::shared_from_this;
    std::shared_ptr<Map<string, std::shared_ptr<PluginState>>> plugins;

    virtual any getPlugin(string id) = 0;
    virtual array<std::shared_ptr<PluginState>> getAllPlugins() = 0;
    virtual array<std::shared_ptr<PluginState>> getLoadedPlugins() = 0;
    virtual void updatePluginState(string id, Partial<std::shared_ptr<PluginState>> update) = 0;
};

class LoadPluginParams : public object, public std::enable_shared_from_this<LoadPluginParams> {
public:
    using std::enable_shared_from_this<LoadPluginParams>::shared_from_this;
    string pluginId;

    boolean force;
};

class UnloadPluginParams : public object, public std::enable_shared_from_this<UnloadPluginParams> {
public:
    using std::enable_shared_from_this<UnloadPluginParams>::shared_from_this;
    string pluginId;
};

class PluginManagerConfig : public object, public std::enable_shared_from_this<PluginManagerConfig> {
public:
    using std::enable_shared_from_this<PluginManagerConfig>::shared_from_this;
    double maxBuildAttempts;

    double buildTimeout;

    string pluginDirectory;

    boolean enableHotReload;
};

class InstallProgress : public object, public std::enable_shared_from_this<InstallProgress> {
public:
    using std::enable_shared_from_this<InstallProgress>::shared_from_this;
    any phase;

    double progress;

    string message;
};

class PluginMetadata : public object, public std::enable_shared_from_this<PluginMetadata> {
public:
    using std::enable_shared_from_this<PluginMetadata>::shared_from_this;
    string name;

    string description;

    string author;

    string repository;

    array<string> versions;

    string latestVersion;

    string runtimeVersion;

    string maintainer;

    array<string> tags;

    array<string> categories;
};

namespace @elizaos/core {
}
#endif
