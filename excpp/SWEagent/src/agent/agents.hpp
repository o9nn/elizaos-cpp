#include ".exceptions.hpp"
#include ".types.hpp"
#include ".utils/log.hpp"
#include "action-sampler.hpp"
#include "history-processors.hpp"
#include "hooks.hpp"
#include "models.hpp"
#include "problem-statement.hpp"
#include "tools/parsing.hpp"
#include "types.hpp"
#include "utils/template.hpp"
#include "utils/yaml.hpp"
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * Main agent classes
 * Converted from sweagent/agent/agents.py
 */

// Import error classes

// Dynamic import for ShellAgent to avoid circular dependency

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
    std::optional<AbstractParseFunction | string> parseFunction;
    double executionTimeout;
    double maxConsecutiveExecutionTimeouts;
    double totalExecutionTimeout;
    std::string submitCommand;
    bool useFunctionCalling;
    std::optional<{> filter;
    std::string blocklistErrorTemplate;
    std::vector<std::string> blocklist;
    std::vector<std::string> blocklistStandalone;
    std::optional<std::unordered_map<std::string, std::string>> blockUnlessRegex;
    std::string formatErrorTemplate;
    std::optional<std::string> commandDocs;
    std::optional<std::unordered_map<std::string, string | number | boolean>> envVariables;
};

/**
 * Tool handler for managing agent tools
 */
class ToolHandler {
  config: ToolConfig;
  tools: CommandBundle[];
  private parser: AbstractParseFunction;
  private multilineCommands: Map<string, string> = new Map();

  constructor(config: ToolConfig) {
    this.config = config;
    this.tools = [];

    // Initialize parser
    if (typeof config.parseFunction === 'string') {
      this.parser = getParser(config.parseFunction);
    } else if (config.parseFunction) {
      this.parser = config.parseFunction;
    } else {
      this.parser = new ThoughtActionParser();
    }

    // Initialize multiline commands from config
    if (config.commands) {
      for (const cmd of config.commands) {
        if (cmd.endName) {
          this.multilineCommands.set(cmd.name, cmd.endName);
        }
      }
    }
  }

    // Install tools in the environment
    // Set environment variables

    // Install each bundle if they exist

      // Process each tool bundle

      // Return to original directory

    // Get current state from environment

    // Get working directory

    // Get open files

    // Get git status if available

    // Parse thought and action from model output using configured parser
    // Use strict mode to throw FormatError when parsing fails

    // Check if action should be blocked

    // Check blocklist

    // Check standalone blocklist

    // Check block unless regex

    // Check if observation contains submission command
    // Look for the special submission marker used by SWE-agent

    // Guard multiline input commands with heredoc syntax
    // This handles commands that can take multiline input

      // Check if action starts with this command
        // Check if it already has the heredoc syntax
          // Add heredoc syntax
            // It's a multiline command that needs guarding

/**
 * Environment interface for agent operations
 */
struct AgentEnvironment {
    std::string command;
    std::optional<number | Record<string, unknown>> timeout;
    std::optional<std::unordered_map<std::string, unknown>> options;
    std::optional<{ repoName: string }> repo;

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
    'default' | 'retry' | 'shell' type;
};

/**
 * Configuration for retry agent
 */
struct RetryAgentConfig {
    std::string name;
    std::vector<DefaultAgentConfig> agentConfigs;
    RetryLoopConfig retryLoop;
    'retry' type;
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
    'shell' type;
};

using AgentConfig = std::variant<DefaultAgentConfig, RetryAgentConfig, ShellAgentConfig>;

/**
 * Abstract base class for agents
 */

/**
 * Default agent implementation
 */
class DefaultAgent extends AbstractAgent {
  name: string;
  model: AbstractModel;
  templates: TemplateConfig;
  tools: ToolHandler;
  historyProcessors: AbstractHistoryProcessor[];
  maxRequeries: number;
  logger: AgentLogger;

  env: AgentEnvironment | null = null;
  problemStatement: ProblemStatement | ProblemStatementConfig | null = null;
  trajPath: string | null = null;

  history: History = [];
  trajectory: Trajectory = [];
  info: AgentInfo = {};

    // Set parser based on model type

      // Initialize action sampler if configured

    // Deep copy config to avoid shared state between different instances

    // Return processed history for this attempt

    // Chain history processors

    // Setup the agent for a new instance

    // Get version from package.json

    // Add system message, demonstrations, and instance template to history

      // Add demonstrations to history step-by-step
      // Add as single message

    // Add instance template to history

    // Get all data saved in .traj files

    // Forward the model without handling errors

      // Query model

        // Parsing probably failed
      // Create a new error with step attached

    // Handle action execution

      // Execute command in environment

    // Check for special tokens in observation

    // Check for submission in observation

        // Handle different error types

        // Errors that cause requery

        // Special retry tokens

        // Errors that cause exit

        // Handle all cost limit errors (base class and derived classes)

        // Handle runtime/environment errors

        // Unknown errors

    // Too many retries

    // Try to extract patch from environment

      // Try to use diff from last trajectory step

    // Try to run submission command

    // Run a single step of the agent

    // Add to history

    // Add observation to history

    // Update info

    // Run the agent on a problem instance

/**
 * Retry agent implementation
 */
class RetryAgent extends AbstractAgent {
  private config: RetryAgentConfig;
  private hooks: AbstractAgentHook[] = [];
  private iAttempt: number = 0;
  private agent: DefaultAgent | null = null;
  private attemptData: Array<{
    trajectory: Trajectory;
    history: History;
    info: AgentInfo;
    replayConfig: string | null;
    environment: string;
  }> = [];

    // Initialize retry loop based on config
    // this.rloop = getRetryLoopFromConfig(this.config.retryLoop, this.problemStatement);

    // Failsafe cost check

        // Submit for review

/**
 * Factory function to get agent from configuration
 */
std::future<AbstractAgent> getAgentFromConfig(AgentConfig config);

} // namespace elizaos
