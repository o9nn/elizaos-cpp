#pragma once
#include <algorithm>
#include <any>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>
#include "action-sampler.hpp"
#include "exceptions.hpp"
#include "history-processors.hpp"
#include "hooks.hpp"
#include "models.hpp"
#include "problem-statement.hpp"
#include "tools/parsing.hpp"
#include "types.hpp"
#include "utils/log.hpp"
#include "utils/template.hpp"
#include "utils/yaml.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * Main agent classes
 * Converted from sweagent/agent/agents.py
 */

// Import error classes

// Special tokens

/**
 * Template configuration for agent messages
 */
struct TemplateConfig {
    std::string systemTemplate;
    std::string instanceTemplate;
    std::string nextStepTemplate;
    std::string nextStepTruncatedObservationTemplate;
    double maxObservationLength;
    std::optional<std::string> nextStepNoOutputTemplate;
    std::optional<std::string> strategyTemplate;
    std::optional<std::string> demonstrationTemplate;
    std::vector<std::string> demonstrations;
    bool putDemosInHistory;
    bool disableImageProcessing;
    std::string shellCheckErrorTemplate;
    std::string commandCancelledTimeoutTemplate;
};

/**
 * Default template values matching Python implementation
 */

/**
 * Command bundle configuration
 */
struct CommandBundle {
    std::string name;
    std::optional<std::string> endName;
    std::optional<std::string> installScript;
};

/**
 * Tool configuration
 */
struct ToolConfig {
    std::vector<CommandBundle> commands;
    std::optional<std::variant<AbstractParseFunction, std::string>> parseFunction;
    double executionTimeout;
    double maxConsecutiveExecutionTimeouts;
    double totalExecutionTimeout;
    std::string submitCommand;
    bool useFunctionCalling;
    std::string blocklistErrorTemplate;
    std::vector<std::string> blocklist;
    std::vector<std::string> blocklistStandalone;
    std::string formatErrorTemplate;
    std::optional<std::string> commandDocs;
};

/**
 * Tool handler for managing agent tools
 */
class ToolHandler {
public:
    ToolHandler(ToolConfig config);
    std::future<void> install(AgentEnvironment env);
    std::future<std::unordered_map<std::string, std::string>> getState(AgentEnvironment env);
    std::tuple<std::string, std::string> parseActions(const std::variant<std::string, ModelOutput>& output);
    bool shouldBlockAction(const std::string& action);
    bool checkForSubmissionCmd(const std::string& observation);
    std::string guardMultilineInput(const std::string& action);

private:
    ToolConfig config_;
    std::vector<CommandBundle> tools_;
    AbstractParseFunction parser_;
};

/**
 * Environment interface for agent operations
 */
struct AgentEnvironment {
    std::optional<std::string> name;
};

/**
 * Logger interface
 */
struct AgentLogger {
};

/**
 * Action sampler configuration interface
 */
struct ActionSamplerConfig {
    std::string type;
};

/**
 * Retry loop configuration interface
 */
struct RetryLoopConfig {
    std::string type;
    double costLimit;
    std::optional<double> maxAttempts;
    std::optional<double> minBudgetForNewAttempt;
};

/**
 * Configuration for default agent
 */
struct DefaultAgentConfig {
    std::string name;
    TemplateConfig templates;
    ToolConfig tools;
    std::vector<AbstractHistoryProcessor> historyProcessors;
    ModelConfig model;
    double maxRequeries;
    std::optional<ActionSamplerConfig> actionSampler;
    std::string type;
};

/**
 * Configuration for retry agent
 */
struct RetryAgentConfig {
    std::string name;
    std::vector<DefaultAgentConfig> agentConfigs;
    RetryLoopConfig retryLoop;
    std::string type;
};

/**
 * Configuration for shell agent
 */
struct ShellAgentConfig {
    std::string name;
    TemplateConfig templates;
    ToolConfig tools;
    std::vector<AbstractHistoryProcessor> historyProcessors;
    ModelConfig model;
    double maxRequeries;
    std::string type;
};

using AgentConfig = std::variant<DefaultAgentConfig, RetryAgentConfig, ShellAgentConfig>;

/**
 * Abstract base class for agents
 */

/**
 * Default agent implementation
 */
class DefaultAgent {
public:
    DefaultAgent(std::optional<std::any> config);
    static DefaultAgent fromConfig(DefaultAgentConfig config);
    void addHook(AbstractAgentHook hook);
    History messages() const;
    void appendHistory(HistoryItem item);
    std::unordered_map<std::string, std::any> getFormatDict(std::optional<std::unordered_map<std::string, std::any>> kwargs);
    void addSystemMessageToHistory();
    void addDemonstrationsToHistory();
    void addDemonstrationToHistory(const std::string& demonstrationPath);
    void addInstanceTemplateToHistory(const std::unordered_map<std::string, std::string>& state);
    Trajectory getTrajectory();
     getTrajectoryData();
    std::future<StepOutput> forward(History history);
    std::future<StepOutput> handleAction(StepOutput step);
    void addStepToTrajectory(StepOutput step);
    std::future<StepOutput> forwardWithHandling(History history);
    History getModelRequeryHistory(const std::string& errorTemplate, StepOutput step, const std::unordered_map<std::string, std::any>& kwargs);
    std::future<StepOutput> attemptAutosubmissionAfterError(StepOutput step);
    std::future<StepOutput> step();

private:
    std::string name_;
    AbstractModel model_;
    TemplateConfig templates_;
    ToolHandler tools_;
    std::vector<AbstractHistoryProcessor> historyProcessors_;
    double maxRequeries_;
    AgentLogger logger_;
    CombinedAgentHook chook_;
};

/**
 * Retry agent implementation
 */
class RetryAgent {
public:
    RetryAgent(RetryAgentConfig config);
    static RetryAgent fromConfig(RetryAgentConfig config);
    void addHook(AbstractAgentHook hook);
    DefaultAgent setupAgent();
    void nextAttempt();
    std::future<StepOutput> step();
    void finalizeAgentRun();
     getTrajectoryData(bool choose = false);
    void saveTrajectory(bool choose = false);

private:
    RetryAgentConfig config_;
    Trajectory trajectory_;
    History history_;
    AgentInfo info_;
    std::string replayConfig_;
    std::string environment_;
    InstanceStats totalInstanceAttemptStats_;
    CombinedAgentHook chook_;
    AgentLogger logger_;
};

/**
 * Factory function to get agent from configuration
 */
std::future<AbstractAgent> getAgentFromConfig(AgentConfig config);

} // namespace elizaos
