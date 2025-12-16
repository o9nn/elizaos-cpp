#include ".exceptions.hpp"
#include ".types.hpp"
#include ".utils/log.hpp"
#include "agents.hpp"
#include "types.hpp"
#include "utils/model-pricing.hpp"
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
 * Model classes and configurations
 * Converted from sweagent/agent/models.py
 */

/**
 * Retry configuration
 */

using RetryConfig = z.infer<typeof RetryConfigSchema>;

/**
 * Generic API model configuration
 */

using GenericAPIModelConfig = z.infer<typeof GenericAPIModelConfigSchema>;

/**
 * Human model configuration
 */

using HumanModelConfig = z.infer<typeof HumanModelConfigSchema>;

/**
 * Replay model configuration
 */

using ReplayModelConfig = z.infer<typeof ReplayModelConfigSchema>;

/**
 * Instant empty submit model configuration
 */
struct InstantEmptySubmitModelConfig {
    'instant_empty_submit' name;
    std::optional<double> delay;
};

/**
 * Human thought model configuration
 */
struct HumanThoughtModelConfig {
    'human_thought' name;
    std::optional<bool> catchEof;
};

/**
 * Union type for all model configurations
 */
using ModelConfig = std::variant<, GenericAPIModelConfig, HumanModelConfig, ReplayModelConfig, InstantEmptySubmitModelConfig, HumanThoughtModelConfig>;

/**
 * Global statistics tracking
 */
class GlobalStats {
  totalCost: number = 0;
  lastQueryTimestamp: number = 0;

  addCost(cost: number): void {
    this.totalCost += cost;
  }

/**
 * Instance-specific statistics
 */
class InstanceStats {
  instanceCost: number = 0;
  tokensSent: number = 0;
  tokensReceived: number = 0;
  apiCalls: number = 0;

  add(other: InstanceStats): InstanceStats {
    const result = new InstanceStats();
    result.instanceCost = this.instanceCost + other.instanceCost;
    result.tokensSent = this.tokensSent + other.tokensSent;
    result.tokensReceived = this.tokensReceived + other.tokensReceived;
    result.apiCalls = this.apiCalls + other.apiCalls;
    return result;
  }

/**
 * Abstract base class for all models
 */

/**
 * Human model for interactive input
 */
class HumanModel extends AbstractModel {
  protected historyPath?: string;
  protected catchEof: boolean;

  constructor(config: HumanModelConfig, tools: ToolConfig) {
    super(config, tools);
    this.historyPath = process.env.HISTFILE || path.join(process.env.HOME || '.', '.swe_agent_history');
    this.catchEof = config.catchEof !== false; // Default to true
    this.loadReadlineHistory();
  }

    // In Node.js, readline history handling is different
    // We'd need to implement custom history management

    // Create readline interface

      // Handle EOF (Ctrl+D)
          // Re-raise EOFError when catchEof is disabled

        // Check if answer is null (EOF)

        // Handle special commands

    // Add other raise commands as needed

/**
 * LiteLLM model for API-based models
 */
class LiteLLMModel extends AbstractModel {
  private apiKeys: string[];
  private currentKeyIndex: number = 0;

  constructor(config: GenericAPIModelConfig, tools: ToolConfig) {
    super(config as ModelConfig, tools);
    this.apiKeys = this.getApiKeys();
  }

      // Try to get from environment

      // It's an environment variable

      // Use thread-based selection

    // Round-robin selection

    struct RequestData {
    std::string model;
    std::optional<std::vector<HistoryItem>> messages;
    double temperature;
    number | null top_p;
    double n;
    std::vector<std::string> stop;
    std::optional<double> max_tokens;
    std::optional<std::string> system;
    std::optional<std::vector<{ role: string; parts: Array<{ text: string }> }>> contents;

    // Determine API endpoint and headers based on provider

      // Convert messages format for Anthropic

      // Convert messages format for Google
      // OpenAI and compatible APIs

      // Convert API response to ModelOutput format

    // Calculate cost (this would need proper model pricing data)

    return calculateCost(config.name, inputTokens, outputTokens);

/**
 * Human model with thought prompting
 */
class HumanThoughtModel extends HumanModel {
  async query(
    _history: History,
    actionPrompt: string | number = '> ',
    n?: number,
  ): Promise<ModelOutput | ModelOutput[]> {
    // First get the thought
    const thoughtPrompt = 'What is your thought? > ';
    const thoughtRl = readline.createInterface({
      input: process.stdin,
      output: process.stdout,
      prompt: thoughtPrompt,
    });

    const thought = await new Promise<string>((resolve) => {
      thoughtRl.question(thoughtPrompt, (answer) => {
        thoughtRl.close();
        resolve(answer);
      });
    });

    // Then get the action
    const actionRl = readline.createInterface({
      input: process.stdin,
      output: process.stdout,
      prompt: typeof actionPrompt === 'string' ? actionPrompt : '> ',
    });

    const action = await new Promise<string>((resolve) => {
      actionRl.question('What is your action? > ', (answer) => {
        actionRl.close();

        if (!this.catchEof && answer === '') {
          throw new EOFError();
        }

        resolve(answer);
      });
    });

    this.updateStats();

    // Handle special commands
    if (action.startsWith('RAISE_')) {
      this.handleRaiseCommands(action);
    }

    const result: ModelOutput = {
      message: `${thought}\n\`\`\`\n${action}\n\`\`\``,
    };

    return n ? [result] : result;
  }

/**
 * Replay model for replaying trajectories
 */
class ReplayModel extends AbstractModel {
  private replays: Array<Array<string | ModelOutput>>;
  private replayIdx: number = 0;
  private actionIdx: number = 0;
  private usesFunctionCalling: boolean;
  private submitCommand: string;

  constructor(config: ReplayModelConfig, tools: ToolConfig) {
    super(config, tools);

    if (!config.replayPath || !fs.existsSync(config.replayPath)) {
      throw new Error(`Replay file ${config.replayPath} not found`);
    }

    const content = fs.readFileSync(config.replayPath, 'utf-8');
    this.replays = content
      .split('\n')
      .filter((line) => line.trim())
      .map((line) => {
        const parsed = JSON.parse(line);
        return Object.values(parsed)[0] as Array<string | ModelOutput>;
      });

    this.usesFunctionCalling = tools.useFunctionCalling;
    this.submitCommand = tools.submitCommand || 'submit';
  }

/**
 * Instant empty submit model for testing
 */
class InstantEmptySubmitModel extends AbstractModel {
  private actionIdx: number = 0;
  private delay: number;

  constructor(config: { name: 'instant_empty_submit'; delay?: number }, tools: ToolConfig) {
    super(config, tools);
    this.delay = config.delay || 0;
  }

    // Add random delay if configured

/**
 * Predetermined test model for testing
 */
class PredeterminedTestModel extends AbstractModel {
  private responses: Array<string | ModelOutput>;
  private responseIdx: number = 0;

  constructor(responses: Array<string | ModelOutput>, tools?: ToolConfig) {
    super({ name: 'test' } as ModelConfig, tools || ({} as ToolConfig));
    this.responses = responses;
  }

    // Handle special test commands
        // Create a RuntimeError-like error that the agent will recognize

// Export types for testing

/**
 * Factory function to create model instances
 */
AbstractModel getModel(ModelConfig config, ToolConfig tools);

} // namespace elizaos
