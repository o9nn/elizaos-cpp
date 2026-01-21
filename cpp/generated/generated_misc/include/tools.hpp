#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SWEAGENT_SRC_TOOLS_TOOLS_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SWEAGENT_SRC_TOOLS_TOOLS_H
#include "core.h"
#include "./bundle.h"
#include "./commands.h"
#include "./parsing.h"
#include "./utils.h"
#include "../environment/swe-env.h"
#include "../utils/log.h"
#include "../types.h"

class ToolFilterConfig;
class ToolConfig;
class ToolHandler;

class ToolFilterConfig : public object, public std::enable_shared_from_this<ToolFilterConfig> {
public:
    using std::enable_shared_from_this<ToolFilterConfig>::shared_from_this;
    std::string blocklistErrorTemplate;

    array<string> blocklist;

    array<string> blocklistStandalone;

    Record<std::string, string> blockUnlessRegex;
};

extern std::shared_ptr<ToolFilterConfig> defaultToolFilterConfig;
class ToolConfig : public object, public std::enable_shared_from_this<ToolConfig> {
public:
    using std::enable_shared_from_this<ToolConfig>::shared_from_this;
    std::shared_ptr<ToolFilterConfig> filter;

    array<std::shared_ptr<Bundle>> bundles;

    array<string> propagateEnvVariables;

    Record<std::string, any> envVariables;

    Record<std::string, any> registryVariables;

    std::string submitCommand;

    std::any parseFunction;

    boolean enableBashTool;

    std::string formatErrorTemplate;

    std::string commandDocs;

    Record<std::string, string> multiLineCommandEndings;

    std::any submitCommandEndName;

    array<any> resetCommands;

    double executionTimeout;

    double installTimeout;

    double totalExecutionTimeout;

    double maxConsecutiveExecutionTimeouts;

    boolean useFunctionCalling;

    array<string> stateCommands;

    array<std::shared_ptr<Command>> commands;

    array<Record<std::string, any>> tools;
};

extern std::shared_ptr<ToolConfig> defaultToolConfig;
class ToolHandler : public object, public std::enable_shared_from_this<ToolHandler> {
public:
    using std::enable_shared_from_this<ToolHandler>::shared_from_this;
    std::shared_ptr<ToolConfig> config;

    std::shared_ptr<AbstractParseFunction> parser;

    std::shared_ptr<Map<std::string, string>> multilineCommands = std::make_shared<Map>();

    std::shared_ptr<AgentLogger> logger;

    ToolHandler(std::shared_ptr<ToolConfig> config = object{});
    virtual array<std::shared_ptr<Command>> getCommandsFromBundles();
    static std::shared_ptr<ToolHandler> fromConfig(std::shared_ptr<ToolConfig> config);
    virtual std::shared_ptr<Promise<void>> install(std::shared_ptr<SWEEnv> env);
    virtual std::shared_ptr<Promise<void>> uploadBundle(std::shared_ptr<SWEEnv> _env, std::shared_ptr<Bundle> bundle);
    virtual std::shared_ptr<Promise<void>> reset(std::shared_ptr<SWEEnv> env);
    virtual std::shared_ptr<Promise<Record<std::string, string>>> getState(std::shared_ptr<SWEEnv> env);
    virtual boolean shouldBlockAction(std::string action);
    virtual boolean checkForSubmissionCmd(std::string observation);
    template <typename P0>
    std::tuple<std::string, string> parseActions(P0 output);
    virtual std::string guardMultilineInput(std::string action);
};

template <typename P0>
std::tuple<std::string, string> ToolHandler::parseActions(P0 output)
{
    auto modelResponse = (type_of(output) == std::string("string")) ? std::any(object{
        object::pair{std::string("message"), output}
    }) : std::any(output);
    return this->parser->parse(modelResponse, OR((this->config->commands), (array<any>())), false);
}

#endif
