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
    string blocklistErrorTemplate;

    array<string> blocklist;

    array<string> blocklistStandalone;

    Record<string, string> blockUnlessRegex;
};

extern std::shared_ptr<ToolFilterConfig> defaultToolFilterConfig;
class ToolConfig : public object, public std::enable_shared_from_this<ToolConfig> {
public:
    using std::enable_shared_from_this<ToolConfig>::shared_from_this;
    std::shared_ptr<ToolFilterConfig> filter;

    array<std::shared_ptr<Bundle>> bundles;

    array<string> propagateEnvVariables;

    Record<string, any> envVariables;

    Record<string, any> registryVariables;

    string submitCommand;

    any parseFunction;

    boolean enableBashTool;

    string formatErrorTemplate;

    string commandDocs;

    Record<string, string> multiLineCommandEndings;

    any submitCommandEndName;

    array<any> resetCommands;

    double executionTimeout;

    double installTimeout;

    double totalExecutionTimeout;

    double maxConsecutiveExecutionTimeouts;

    boolean useFunctionCalling;

    array<string> stateCommands;

    array<std::shared_ptr<Command>> commands;

    array<Record<string, any>> tools;
};

extern std::shared_ptr<ToolConfig> defaultToolConfig;
class ToolHandler : public object, public std::enable_shared_from_this<ToolHandler> {
public:
    using std::enable_shared_from_this<ToolHandler>::shared_from_this;
    std::shared_ptr<ToolConfig> config;

    std::shared_ptr<AbstractParseFunction> parser;

    std::shared_ptr<Map<string, string>> multilineCommands = std::make_shared<Map>();

    std::shared_ptr<AgentLogger> logger;

    ToolHandler(std::shared_ptr<ToolConfig> config = object{});
    virtual array<std::shared_ptr<Command>> getCommandsFromBundles();
    static std::shared_ptr<ToolHandler> fromConfig(std::shared_ptr<ToolConfig> config);
    virtual std::shared_ptr<Promise<void>> install(std::shared_ptr<SWEEnv> env);
    virtual std::shared_ptr<Promise<void>> uploadBundle(std::shared_ptr<SWEEnv> _env, std::shared_ptr<Bundle> bundle);
    virtual std::shared_ptr<Promise<void>> reset(std::shared_ptr<SWEEnv> env);
    virtual std::shared_ptr<Promise<Record<string, string>>> getState(std::shared_ptr<SWEEnv> env);
    virtual boolean shouldBlockAction(string action);
    virtual boolean checkForSubmissionCmd(string observation);
    template <typename P0>
    std::tuple<string, string> parseActions(P0 output);
    virtual string guardMultilineInput(string action);
};

template <typename P0>
std::tuple<string, string> ToolHandler::parseActions(P0 output)
{
    auto modelResponse = (type_of(output) == std::string("string")) ? any(object{
        object::pair{std::string("message"), output}
    }) : any(output);
    return this->parser->parse(modelResponse, OR((this->config->commands), (array<any>())), false);
}

#endif
