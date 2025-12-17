#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include ".environment/swe-env.hpp"
#include ".types.hpp"
#include ".utils/log.hpp"
#include "bundle.hpp"
#include "commands.hpp"
#include "parsing.hpp"
#include "utils.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * Tools configuration and handler
 * Converted from sweagent/tools/tools.py
 */

/**
 * Tool filter configuration
 */
struct ToolFilterConfig {
    std::string blocklistErrorTemplate;
    std::vector<std::string> blocklist;
    std::vector<std::string> blocklistStandalone;
};

/**
 * Default tool filter config
 */

/**
 * Tool configuration
 */
struct ToolConfig {
    std::optional<ToolFilterConfig> filter;
    std::optional<std::vector<Bundle>> bundles;
    std::optional<std::vector<std::string>> propagateEnvVariables;
    std::optional<std::string> submitCommand;
    std::optional<std::variant<AbstractParseFunction, std::string>> parseFunction;
    std::optional<bool> enableBashTool;
    std::optional<std::string> formatErrorTemplate;
    std::optional<std::string> commandDocs;
    std::optional<std::optional<std::string>> submitCommandEndName;
    std::optional<std::variant<Array<string, string[]>>> resetCommands;
    std::optional<double> executionTimeout;
    std::optional<double> installTimeout;
    std::optional<double> totalExecutionTimeout;
    std::optional<double> maxConsecutiveExecutionTimeouts;
    std::optional<bool> useFunctionCalling;
    std::optional<std::vector<std::string>> stateCommands;
    std::optional<std::vector<Command>> commands;
};

/**
 * Default tool configuration
 */

/**
 * Tool handler
 */
class ToolHandler {
public:
    ToolHandler(ToolConfig = {} config);
    std::vector<Command> getCommandsFromBundles();
    ToolHandler fromConfig(ToolConfig config);
    std::future<void> install(SWEEnv env);
    std::future<void> uploadBundle(SWEEnv _env, Bundle bundle);
    std::future<void> reset(SWEEnv env);
    bool shouldBlockAction(const std::string& action);
    bool checkForSubmissionCmd(const std::string& observation);
    std::string guardMultilineInput(const std::string& action);


} // namespace elizaos
