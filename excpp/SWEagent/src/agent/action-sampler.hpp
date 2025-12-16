#include ".types.hpp"
#include ".utils/log.hpp"
#include "agents.hpp"
#include "models.hpp"
#include "problem-statement.hpp"
#include "types.hpp"
#include "utils/template.hpp"
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
 * Action sampling strategies for agent decision making
 * Converted from sweagent/agent/action_sampler.py
 */

;
;
;
;
;
;
;

const logger = getLogger('action-sampler');

/**
 * Output from action sampler
 */
struct ActionSamplerOutput {
    ModelOutput completion;
    History messages;
    Trajectory trajectoryItems;
    std::unordered_map<std::string, unknown> extraInfo;
};


/**
 * Abstract base class for action samplers
 */
abstract class AbstractActionSampler {
  protected model: AbstractModel;
  protected tools: ToolHandler;
  protected logger: AgentLogger;

  constructor(model: AbstractModel, tools: ToolHandler) {
    this.model = model;
    this.tools = tools;
    this.logger = logger;
  }

  /**
   * Get action from the sampler
   */
  abstract getAction(
    _problemStatement: ProblemStatement,
    _trajectory: Trajectory,
    history: History,
  ): Promise<ActionSamplerOutput>;
}

/**
 * Configuration for AskColleagues sampler
 */
struct AskColleaguesConfig {
    'ask_colleagues' type;
    double nSamples;
};


/**
 * AskColleagues action sampler
 * Queries multiple model instances and synthesizes their responses
 */
class AskColleagues extends AbstractActionSampler {
  private config: AskColleaguesConfig;

  constructor(config: AskColleaguesConfig, model: AbstractModel, tools: ToolHandler) {
    super(model, tools);
    this.config = config;
  }

  static fromConfig(config: AskColleaguesConfig, model: AbstractModel, tools: ToolHandler): AskColleagues {
    return new AskColleagues(config, model, tools);
  }

  private getColleagueDiscussion(completions: ModelOutput[]): string {
    let out = 'Your colleagues had the following ideas: \n\n';
    let nParsedOk = 0;

    for (let i = 0; i < completions.length; i++) {
      try {
        const [thought, action] = this.tools.parseActions(completions[i]);
        nParsedOk++;
        out += `Thought (colleague ${i}): ${thought}\nProposed Action (colleague ${i}): ${action}\n\n`;
      } catch (error) {
        this.logger.warning(`Could not parse completion ${completions[i]}, skipping.`);
        continue;
      }
    }

    if (nParsedOk === 0) {
      throw new Error('No completions could be parsed.');
    }

    out +=
      'Please summarize and compare the ideas and propose and action to take. ' +
      'Finally choose one action to perform and explain it in detail and include it as a tool call. ' +
      '<important>You must include a thought and action (as a tool/`);

    // Create new messages with discussion
    const newMessages = [{ role: 'user', content: discussion, messageType: 'user' as const }];

    // Get final completion
    const finalCompletion = (await this.model.query([...history, ...newMessages])) as ModelOutput;

    return {
      completion: finalCompletion,
      messages: [],
      trajectoryItems: [],
      extraInfo: { colleagues: discussion },
    };
  }
}

/**
 * Configuration for BinaryTrajectoryComparison sampler
 */
struct BinaryTrajectoryComparisonConfig {
    'binary_trajectory_comparison' type;
    double minNSamples;
    double maxNSamples;
    std::optional<double> comparisonTemperature;
    std::string systemTemplate;
    std::string instanceTemplate;
    std::string comparisonTemplate;
};


/**
 * BinaryTrajectoryComparison action sampler
 * Compares pairs of proposed actions and selects the best one
 */
class BinaryTrajectoryComparison extends AbstractActionSampler {
  private config: BinaryTrajectoryComparisonConfig;

  constructor(config: BinaryTrajectoryComparisonConfig, model: AbstractModel, tools: ToolHandler) {
    super(model, tools);
    this.config = config;
  }

  static fromConfig(
    config: BinaryTrajectoryComparisonConfig,
    model: AbstractModel,
    tools: ToolHandler,
  ): BinaryTrajectoryComparison {
    return new BinaryTrajectoryComparison(config, model, tools);
  }

  private formatTrajectory(trajectory: Trajectory): string {
    const steps: string[] = [];
    for (let i = 0; i < trajectory.length; i++) {
      const step = trajectory[i];
      steps.push(`Action ${i}: ${step.action}\n Observation ${i}: ${step.observation}`);
    }
    return steps.join('\n');
  }

  private formatMessages(params: {
    problemStatement: ProblemStatement;
    trajectory: Trajectory;
    thought1: string;
    action1: string;
    thought2: string;
    action2: string;
    useCacheControl?: boolean;
  }): History {
    const systemMessage = this.config.systemTemplate;
    this.logger.debug(`MODEL INPUT (system)\n${systemMessage}`);

    const userMessage = renderAdvancedTemplate(this.config.instanceTemplate, {
      problem_statement: params.problemStatement.getProblemStatement(),
      ...params.problemStatement.getExtraFields(),
      traj: this.formatTrajectory(params.trajectory),
    });

    const comparisonMessage = renderTemplate(this.config.comparisonTemplate, {
      thought1: params.thought1,
      action1: params.action1,
      thought2: params.thought2,
      action2: params.action2,
    });

    const messages: History = [
      { role: 'system', content: systemMessage, messageType: 'system' },
      { role: 'user', content: userMessage, messageType: 'user' },
      { role: 'user', content: comparisonMessage, messageType: 'user' },
    ];

    if (params.useCacheControl) {
      // Add cache control to messages
      const userMessageContent = {
        type: 'text' as const,
        text: userMessage,
        cacheControl: { type: 'ephemeral' as const },
      };
      messages[1].content = [userMessageContent];
    }

    return messages;
  }

  private filterDuplicates(completions: ModelOutput[]): ModelOutput[] {
    const thoughts: string[] = [];
    const actions: string[] = [];
    const filteredCompletions: ModelOutput[] = [];

    for (const pc of completions) {
      const [thought, action] = this.tools.parseActions(pc);
      if (!actions.includes(action)) {
        thoughts.push(thought);
        actions.push(action);
        filteredCompletions.push(pc);
      }
    }

    if (filteredCompletions.length < completions.length) {
      this.logger.debug(`Filtering duplicates: ${completions.length} -> ${filteredCompletions.length}`);
    }

    return filteredCompletions;
  }

  private filterParseableCompletions(completions: ModelOutput[]): ModelOutput[] {
    const filteredCompletions: ModelOutput[] = [];

    for (const completion of completions) {
      try {
        this.tools.parseActions(completion);
        filteredCompletions.push(completion);
      } catch (error) {
        this.logger.warning(`Could not parse completion ${completion}, skipping.`);
        continue;
      }
    }

    if (filteredCompletions.length === 0) {
      throw new Error('No completions could be parsed.');
    }

    return filteredCompletions;
  }

  private containsEdits(completions: ModelOutput[]): boolean {
    const keywords = ['edit', 'str_replace_editor insert', 'str_replace_editor str_replace'];

    for (const completion of completions) {
      const [, action] = this.tools.parseActions(completion);
      if (keywords.some((keyword) => action.startsWith(keyword))) {
        return true;
      }
    }

    return false;
  }

  private async getCompletions(history: History): Promise<ModelOutput[]> {
    let completions = (await this.model.query(history, undefined, this.config.minNSamples)) as ModelOutput[];
    completions = this.filterParseableCompletions(completions);
    completions = this.filterDuplicates(completions);

    if (!completions.length) {
      throw new Error('No completions could be parsed.');
    }

    if (this.containsEdits(completions) && this.config.minNSamples < this.config.maxNSamples) {
      this.logger.debug('Edits were proposed, will sample more');
      const newCompletions = (await this.model.query(
        history,
        undefined,
        this.config.maxNSamples - this.config.minNSamples,
      )) as ModelOutput[];
      completions = this.filterDuplicates(this.filterParseableCompletions([...completions, ...newCompletions]));
    }

    if (completions.length === 1) {
      const [, action] = this.tools.parseActions(completions[0]);
      this.logger.warning(`Only identical actions were proposed (action=${action})`);
    }

    return completions;
  }

  private interpret(response: string): 0 | 1 {
    const lastLine = response.trim().split('\n').pop()?.trim() || '';

    if (lastLine.toLowerCase().includes('first')) {
      return 0;
    } else if (lastLine.toLowerCase().includes('second')) {
      return 1;
    }

    this.logger.warning(`Could not interpret response: ${response}, will choose first submission.`);
    return 0;
  }

  async getAction(
    _problemStatement: ProblemStatement,
    _trajectory: Trajectory,
    history: History,
  ): Promise<ActionSamplerOutput> {
    const completions = await this.getCompletions(history);
    let bestIdx = 0;

    struct ComparisonLogEntry {
    [number, number] comparisonBetween;
    History messages;
    std::string response;
    double idx;
};


    const comparisonLog: ComparisonLogEntry[] = [];

    for (let i = 1; i < completions.length; i++) {
      const [thought1, action1] = this.tools.parseActions(completions[bestIdx]);
      const [thought2, action2] = this.tools.parseActions(completions[i]);

      const messages = this.formatMessages({
        problemStatement: _problemStatement,
        trajectory: _trajectory,
        thought1,
        action1,
        thought2,
        action2,
        useCacheControl: completions.length >= 3,
      });

      const response = (await this.model.query(messages, this.config.comparisonTemperature, 1)) as ModelOutput;

      this.logger.info(`RESPONSE: ${response.message}`);
      const idx = this.interpret(response.message);

      comparisonLog.push({
        comparisonBetween: [bestIdx, i],
        messages,
        response: response.message,
        idx,
      });

      bestIdx = idx === 1 ? i : bestIdx;
    }

    return {
      completion: completions[bestIdx],
      messages: [],
      trajectoryItems: [],
      extraInfo: { comparisonLog },
    };
  }
}

/**
 * Base action sampler configuration
 */
struct BaseActionSamplerConfig {
    std::string type;
};


/**
 * Union type for action sampler configurations
 */
using ActionSamplerConfig = std::variant<, (BinaryTrajectoryComparisonConfig & BaseActionSamplerConfig), (AskColleaguesConfig & BaseActionSamplerConfig)>;

/**
 * Factory `);
  }
}

/**
 * Add get method to configs for compatibility
 */
>(
  config: T,
): T & BaseActionSamplerConfig {
  return {
    ...config,
    get: (model: AbstractModel, tools: ToolHandler) =>
      createActionSampler(config as unknown as ActionSamplerConfig, model, tools),
  };
}

} // namespace elizaos
