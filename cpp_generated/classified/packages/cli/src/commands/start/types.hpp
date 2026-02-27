#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_CLI_SRC_COMMANDS_START_TYPES_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_CLI_SRC_COMMANDS_START_TYPES_H
#include "core.h"
#include "@elizaos/core.h"
#include "@elizaos/server.h"

class StartOptions;
class AgentStartConfig;
class AgentStartOptions;
class PluginValidation;
class PluginContext;
class ServerConfig;
class RuntimeConfig;

class StartOptions : public object, public std::enable_shared_from_this<StartOptions> {
public:
    using std::enable_shared_from_this<StartOptions>::shared_from_this;
    double port;

    string configDir;
};

class AgentStartConfig : public object, public std::enable_shared_from_this<AgentStartConfig> {
public:
    using std::enable_shared_from_this<AgentStartConfig>::shared_from_this;
    std::shared_ptr<Character> character;

    std::shared_ptr<AgentServer> server;

    std::function<std::shared_ptr<Promise<void>>(std::shared_ptr<IAgentRuntime>)> init;

    array<any> plugins;

    std::shared_ptr<AgentStartOptions> options;
};

class AgentStartOptions : public object, public std::enable_shared_from_this<AgentStartOptions> {
public:
    using std::enable_shared_from_this<AgentStartOptions>::shared_from_this;
    boolean isTestMode;
};

class PluginValidation : public object, public std::enable_shared_from_this<PluginValidation> {
public:
    using std::enable_shared_from_this<PluginValidation>::shared_from_this;
    boolean isValid;

    std::shared_ptr<Plugin> plugin;

    string error;
};

class PluginContext : public object, public std::enable_shared_from_this<PluginContext> {
public:
    using std::enable_shared_from_this<PluginContext>::shared_from_this;
    boolean isLocalDevelopment;

    string name;

    string path;
};

class ServerConfig : public object, public std::enable_shared_from_this<ServerConfig> {
public:
    using std::enable_shared_from_this<ServerConfig>::shared_from_this;
    double port;

    string configDir;

    boolean skipBuild;
};

class RuntimeConfig : public object, public std::enable_shared_from_this<RuntimeConfig> {
public:
    using std::enable_shared_from_this<RuntimeConfig>::shared_from_this;
    array<std::shared_ptr<Plugin>> plugins;

    std::shared_ptr<Character> character;

    std::shared_ptr<AgentServer> server;
};

#endif
