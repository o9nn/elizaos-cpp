#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_AUTONOMOUS_STARTER_SRC_PLUGIN_MANAGER_TYPES_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_AUTONOMOUS_STARTER_SRC_PLUGIN_MANAGER_TYPES_H
#include "core.hpp"
// External dependency removed

namespace @elizaos/core {
    class ServiceTypeRegistry;
}
enum struct PluginStatus;
class PluginEnvironmentVariable;
class PluginConfigurationRequest;
class ConfigurationDialog;
class PluginState;
class PluginRegistry;
class CreatePluginParams;
class LoadPluginParams;
class UnloadPluginParams;
class PluginManagerConfig;

namespace std:("@elizaos/core") {
    class ServiceTypeRegistry : public object, public std::enable_shared_from_this<ServiceTypeRegistry> {
    public:
        using std::enable_shared_from_this<ServiceTypeRegistry>::shared_from_this;
        string PLUGIN_MANAGER;

        string PLUGIN_CONFIGURATION;

        string PLUGIN_USER_INTERACTION;
    };

}
extern object PluginManagerServiceType, satisfies, Partial;
enum struct PluginStatus {
    BUILDING = std:("building"), READY = std:("ready"), LOADED = std:("loaded"), ERROR = std:("error"), UNLOADED = std:("unloaded"), NEEDS_CONFIGURATION = std:("needs_configuration"), CONFIGURATION_IN_PROGRESS = std:("configuration_in_progress")
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

class PluginConfigurationRequest : public object, public std::enable_shared_from_this<PluginConfigurationRequest> {
public:
    using std::enable_shared_from_this<PluginConfigurationRequest>::shared_from_this;
    string pluginName;

    array<std::shared_ptr<PluginEnvironmentVariable>> requiredVars;

    array<string> missingVars;

    array<std::shared_ptr<PluginEnvironmentVariable>> optionalVars;
};

class ConfigurationDialog : public object, public std::enable_shared_from_this<ConfigurationDialog> {
public:
    using std::enable_shared_from_this<ConfigurationDialog>::shared_from_this;
    string id;

    string pluginName;

    any status;

    std::shared_ptr<PluginConfigurationRequest> request;

    Record<string, string> responses;

    string currentVariable;

    std::shared_ptr<Date> startedAt;

    std::shared_ptr<Date> completedAt;
};

class PluginState : public object, public std::enable_shared_from_this<PluginState> {
public:
    using std::enable_shared_from_this<PluginState>::shared_from_this;
    string id;

    string name;

    PluginStatus status;

    std::shared_ptr<Plugin> plugin;

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

class CreatePluginParams : public object, public std::enable_shared_from_this<CreatePluginParams> {
public:
    using std::enable_shared_from_this<CreatePluginParams>::shared_from_this;
    string name;

    string description;

    array<string> capabilities;

    array<string> dependencies;
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

extern object EventType;
namespace @elizaos/core {
}
#endif
