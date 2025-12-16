#include ".types.hpp"
#include ".utils/log.hpp"
#include "agents.hpp"
#include "models.hpp"
#include "problem-statement.hpp"
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
 * Reviewer and retry loop implementations for agent solutions
 * Converted from sweagent/agent/reviewer.py
 */

;
;
;
;
;
;

const logger = getLogger('reviewer');

/**
 * Information passed to the reviewer
 */
struct ReviewSubmission {
    Trajectory trajectory;
    AgentInfo info;
    InstanceStats modelStats;
};


class ReviewSubmissionImpl implements ReviewSubmission {
  trajectory: Trajectory;
  info: AgentInfo;
  modelStats: InstanceStats;

  constructor(data: { trajectory: Trajectory; info: AgentInfo; modelStats: InstanceStats }) {
    this.trajectory = data.trajectory;
    this.info = data.info;
    this.modelStats = data.modelStats;
  }

  toFormatDict(suffix: string = ''): Record<string, unknown> {
    const out: Record<string, unknown> = {};
    const info = { ...this.info };

    if (!info.submission) {
      info.submission = '';
    }

    for (const [k, v] of Object.entries(info)) {
      if (typeof v === 'string') {
        out[`${k}${suffix}`] = v;
      } else if (typeof v === 'object' && v !== null) {
        for (const [k2, v2] of Object.entries(v)) {
          out[`${k}_${k2}${suffix}`] = v2;
        }
      }
    }

    return out;
  }
}

/**
 * Result from reviewer
 */
struct ReviewerResult {
    boolean | number accept;
    std::vector<std::string> outputs;
    History messages;
};


/**
 * Output from preselector
 */
struct PreselectorOutput {
    std::vector<double> chosenIdx;
    std::string response;
    History messages;
};


/**
 * Output from chooser
 */
struct ChooserOutput {
    double chosenIdx;
    std::string response;
    std::optional<PreselectorOutput | null> preselectorOutput;
    History messages;
};


/**
 * Abstract reviewer interface
 */
abstract class AbstractReviewer {
  abstract review(instance: ProblemStatement, submission: ReviewSubmission): Promise<ReviewerResult>;
}

/**
 * Abstract retry loop interface
 */
abstract class AbstractRetryLoop {
  retry(): boolean {
    return false;
  }

  onSubmit(_submission: ReviewSubmission): void {
    // Default implementation - do nothing
  }

  onModelQuery(_attemptStats: InstanceStats): void {
    // Default implementation - do nothing
  }

  onAttemptStarted(_iAttempt: number, _agent: unknown): void {
    // Default implementation - do nothing
  }

  abstract getBest(): number | null | Promise<number | null>;

  getForwardedVars(): Record<string, unknown> {
    return {};
  }
}

/**
 * Configuration for trajectory formatter
 */
struct TrajFormatterConfig {
    std::vector<std::string> filter;
    std::vector<std::string> outputFilter;
    std::string itemTemplate;
    double onlyShowLastNOutput;
};


/**
 * Configuration for reviewer
 */
struct ReviewerConfig {
    std::string systemTemplate;
    std::string instanceTemplate;
    double failureScorePenalty;
    TrajFormatterConfig trajFormatter;
    double nSample;
    double reduceByStd;
    [number | null, number | null] scoreRange;
    'reviewer' type;
};


/**
 * Configuration for chooser
 */
struct ChooserConfig {
    ModelConfig model;
    std::string systemTemplate;
    std::string instanceTemplate;
    std::string submissionTemplate;
    double maxLenSubmission;
    std::optional<PreselectorConfig | null> preselector;
};


/**
 * Configuration for preselector
 */
struct PreselectorConfig {
    ModelConfig model;
    std::string systemTemplate;
    std::string instanceTemplate;
    std::string submissionTemplate;
    double maxLenSubmission;
};


/**
 * Configuration for chooser retry loop
 */
struct ChooserRetryLoopConfig {
    'chooser' type;
    ChooserConfig chooser;
    double maxAttempts;
    double minBudgetForNewAttempt;
    double costLimit;
};


/**
 * Configuration for score retry loop
 */
struct ScoreRetryLoopConfig {
    'score' type;
    ReviewerConfig reviewerConfig;
    double acceptScore;
    double maxAccepts;
    double maxAttempts;
    double minBudgetForNewAttempt;
    double costLimit;
    ModelConfig model;
};


using RetryLoopConfig = std::variant<ScoreRetryLoopConfig, ChooserRetryLoopConfig>;

/**
 * Trajectory formatter for use in prompts
 */
class TrajectoryFormatter {
  private config: TrajFormatterConfig;

  constructor(config: TrajFormatterConfig) {
    this.config = config;
  }

  private includeStep(item: TrajectoryStep): boolean {
    const action = item.action.trim();
    for (const f of this.config.filter) {
      if (action.startsWith(f)) {
        return false;
      }
    }
    return true;
  }

  private includeStepOutput(item: TrajectoryStep, iStep: number, nSteps: number): boolean {
    if (this.config.onlyShowLastNOutput > 0 && iStep < nSteps - this.config.onlyShowLastNOutput) {
      return false;
    }

    const action = item.action.trim();
    for (const f of this.config.outputFilter) {
      if (action.startsWith(f)) {
        return false;
      }
    }
    return true;
  }

  private formatTrajectoryStep(step: TrajectoryStep, iStep: number, nSteps: number, iTraj: number = 1): string {
    const stepCopy = { ...step };

    if (!this.includeStepOutput(step, iStep, nSteps)) {
      stepCopy.observation = '[Output omitted]';
    }

    return renderTemplate(this.config.itemTemplate, {
      ...stepCopy,
      i_step: iStep,
      i_traj: iTraj,
    });
  }

  formatTrajectory(trajectory: Trajectory, iTraj: number = 1): string {
    const trajMessages = trajectory.filter((step) => this.includeStep(step));

    return trajMessages
      .map((step, iStep) => this.formatTrajectoryStep(step, iStep, trajMessages.length, iTraj))
      .join('\n\n');
  }
}

/**
 * Reviewer implementation
 */
class Reviewer extends AbstractReviewer {
  private config: ReviewerConfig;
  private model: AbstractModel;
  private trajFormatter: TrajectoryFormatter;
  private logger: AgentLogger;

  constructor(config: ReviewerConfig, model: AbstractModel) {
    super();
    this.config = config;
    this.model = model;
    this.trajFormatter = new TrajectoryFormatter(config.trajFormatter);
    this.logger = logger;
  }

  private formatMessages(instance: ProblemStatement, submission: ReviewSubmission): History {
    const systemMessage = this.config.systemTemplate;
    this.logger.debug(`MODEL INPUT (system)\n${systemMessage}`);

    const userMessage = renderAdvancedTemplate(this.config.instanceTemplate, {
      problem_statement: instance.getProblemStatement(),
      ...instance.getExtraFields(),
      ...submission.toFormatDict(),
      traj: this.trajFormatter.formatTrajectory(submission.trajectory),
    });

    this.logger.debug(`MODEL INPUT (user)\n${userMessage}`);

    return [
      { role: 'system', content: systemMessage, messageType: 'system' },
      { role: 'user', content: userMessage, messageType: 'user' },
    ];
  }

  private interpret(response: string): number {
    const lastLine = response.trim().split('\n').pop()?.trim() || '';
    const numbers = lastLine.match(/-?\d+\.?\d*/g);

    if (!numbers || numbers.length === 0) {
      throw new Error(`Could not interpret response: '${lastLine}'`);
    }

    const score = parseFloat(numbers[numbers.length - 1]);

    if (this.config.scoreRange[0] !== null && score < this.config.scoreRange[0]) {
      throw new Error(`Score ${score} is below the minimum score ${this.config.scoreRange[0]}`);
    }

    if (this.config.scoreRange[1] !== null && score > this.config.scoreRange[1]) {
      throw new Error(`Score ${score} is above the maximum score ${this.config.scoreRange[1]}`);
    }

    return score;
  }

  async review(instance: ProblemStatement, submission: ReviewSubmission): Promise<ReviewerResult> {
    const exitStatus = submission.info.exitStatus;
    let penalty = 0.0;

    if (!exitStatus || exitStatus.trim() !== 'submitted') {
      penalty = this.config.failureScorePenalty;
    }

    const messages = this.formatMessages(instance, submission);

    const answers: string[] = [];
    const accepts: number[] = [];

    for (let i = 0; i < this.config.nSample; i++) {
      try {
        const answer = await this.model.query(messages);
        const message = Array.isArray(answer) ? answer[0].message : answer.message;
        const score = this.interpret(message);
        answers.push(message);
        accepts.push(score);
      } catch (error) {
        this.logger.warning(`Could not interpret response: ${error}`);
        continue;
      }
    }

    if (accepts.length === 0) {
      answers.push('No valid scores found, failing submission');
      accepts.push(-100.0);
    }

    const mean = accepts.reduce((a, b) => a + b, 0) / accepts.length;
    const std = Math.sqrt(accepts.reduce((sq, n) => sq + Math.pow(n - mean, 2), 0) / accepts.length);

    let accept = mean - penalty;
    if (this.config.reduceByStd > 0) {
      accept -= std * this.config.reduceByStd;
    }

    this.logger.info(`First answer: ${answers[0]}`);
    this.logger.info(`Final score: ${accept} (penalty: ${penalty}, std: ${std}), individual: ${accepts}`);

    return {
      accept,
      outputs: answers,
      messages,
    };
  }
}

/**
 * Chooser implementation
 */
class Chooser {
  private config: ChooserConfig;
  private model: AbstractModel;
  private logger: AgentLogger;

  constructor(config: ChooserConfig) {
    this.config = config;
    // The model doesn't need tool configuration for reviewer
    // The model doesn't need tool configuration for reviewer
    this.model = getModel(config.model, {} as ToolConfig);
    this.logger = logger;
  }

  private interpret(response: string): number {
    const numbers = response.match(/\d+/g);
    if (!numbers || numbers.length === 0) {
      this.logger.error('No numbers found in response');
      return 0;
    }
    return parseInt(numbers[numbers.length - 1]);
  }

  private formatSubmission(_problemStatement: string, submission: ReviewSubmission): string {
    if (
      !submission.info.submission ||
      (this.config.maxLenSubmission > 0 && submission.info.submission.length > this.config.maxLenSubmission)
    ) {
      return 'Solution invalid.';
    }

    return renderTemplate(this.config.submissionTemplate, {
      ...submission.toFormatDict(),
    });
  }

  private buildMessages(problemStatement: string, input: ReviewSubmission[]): History {
    const instanceMessage = renderAdvancedTemplate(this.config.instanceTemplate, {
      problem_statement: problemStatement,
      submissions: input.map((s) => this.formatSubmission(problemStatement, s)),
    });

    this.logger.debug(`MODEL INPUT (user)\n${instanceMessage}`);

    return [
      { role: 'system', content: this.config.systemTemplate, messageType: 'system' },
      { role: 'user', content: instanceMessage, messageType: 'user' },
    ];
  }

  async choose(problemStatement: string, input: ReviewSubmission[]): Promise<ChooserOutput> {
    const preselectorOutput: PreselectorOutput | null = null;
    let selectedIndices = Array.from({ length: input.length }, (_, i) => i);

    const nSubmitted = input.filter((s) => s.info.exitStatus === 'submitted').length;

    if (nSubmitted >= 2) {
      this.logger.debug(`Got ${nSubmitted} submitted submissions, only using them`);
      selectedIndices = input.map((s, i) => (s.info.exitStatus === 'submitted' ? i : -1)).filter((i) => i >= 0);
    } else {
      this.logger.debug(`Got only ${nSubmitted} submitted submissions, disabling exit status filtering`);
    }

    // Handle preselector if configured
    if (this.config.preselector && selectedIndices.length > 2) {
      // Preselector logic would go here
    }

    const messages = this.buildMessages(
      problemStatement,
      selectedIndices.map((i) => input[i]),
    );

    let chosenIdx: number | null = null;
    let response = '';

    try {
      const result = await this.model.query(messages);
      response = Array.isArray(result) ? result[0].message : result.message;
      chosenIdx = this.interpret(response);
    } catch (error) {
      this.logger.critical(`Chooser failed: ${error}`);
      chosenIdx = null;
    }

    if (chosenIdx === null || chosenIdx < 0 || chosenIdx >= selectedIndices.length) {
      this.logger.error(`Invalid chosen index: ${chosenIdx}, using first index`);
      chosenIdx = selectedIndices[0];
    } else {
      chosenIdx = selectedIndices[chosenIdx];
    }

    return {
      chosenIdx,
      response,
      preselectorOutput,
      messages,
    };
  }
}

/**
 * Chooser retry loop implementation
 */
class ChooserRetryLoop extends AbstractRetryLoop {
  private config: ChooserRetryLoopConfig;
  private problemStatement: ProblemStatement;
  private chooser: Chooser;
  private submissions: ReviewSubmission[] = [];
  // private __nConsecExitCost: number = 0; // Unused
  private logger: AgentLogger;
  private chooserOutput: ChooserOutput | null = null;

  constructor(config: ChooserRetryLoopConfig, problemStatement: ProblemStatement) {
    super();
    this.config = config;
    this.problemStatement = problemStatement;
    this.chooser = new Chooser(config.chooser);
    this.logger = logger;
  }

  private get totalStats(): InstanceStats {
    return this.submissions.reduce((acc, s) => acc.add(s.modelStats), new InstanceStats());
  }

  get reviewModelStats(): InstanceStats {
    return new InstanceStats();
  }

  private get nAttempts(): number {
    return this.submissions.length;
  }

  onSubmit(_submission: ReviewSubmission): void {
    this.submissions.push(_submission);
  }

  retry(): boolean {
    const statStr = `n_samples=${this.nAttempts}`;

    if (this.config.costLimit > 0 && this.totalStats.instanceCost > this.config.costLimit) {
      this.logger.info(
        `Exiting retry loop (${statStr}): Total attempt cost ` +
          `(${this.totalStats.instanceCost}) exceeds cost limit (${this.config.costLimit})`,
      );
      return false;
    }

    if (this.config.maxAttempts > 0 && this.nAttempts >= this.config.maxAttempts) {
      this.logger.info(`Exiting retry loop (${statStr}): max_attempts=${this.config.maxAttempts} reached`);
      return false;
    }

    const remainingBudget = this.config.costLimit - this.totalStats.instanceCost;
    if (this.config.minBudgetForNewAttempt > 0 && remainingBudget < this.config.minBudgetForNewAttempt) {
      this.logger.info(
        `Exiting retry loop (${statStr}): Not enough budget left for a new attempt ` +
          `(${remainingBudget} remaining, ${this.config.minBudgetForNewAttempt} required)`,
      );
      return false;
    }

    return true;
  }

  async getBest(): Promise<number | null> {
    if (this.chooserOutput !== null) {
      return this.chooserOutput.chosenIdx;
    }

    if (this.submissions.length === 0) {
      return null;
    }

    this.chooserOutput = await this.chooser.choose(this.problemStatement.getProblemStatement(), this.submissions);

    return this.chooserOutput.chosenIdx;
  }
}

/**
 * Score retry loop implementation
 */
class ScoreRetryLoop extends AbstractRetryLoop {
  private model: AbstractModel;
  private problemStatement: ProblemStatement;
  private reviewer: AbstractReviewer;
  private config: ScoreRetryLoopConfig;
  private submissions: ReviewSubmission[] = [];
  private reviews: ReviewerResult[] = [];
  private nConsecExitCost: number = 0;
  private logger: AgentLogger;

  constructor(config: ScoreRetryLoopConfig, problemStatement: ProblemStatement) {
    super();
    this.config = config;
    this.problemStatement = problemStatement;
    // The model doesn't need tool configuration for reviewer
    this.model = getModel(config.model, {} as ToolConfig);
    this.reviewer = new Reviewer(config.reviewerConfig, this.model);
    this.logger = logger;
  }

  get reviewModelStats(): InstanceStats {
    return this.model.getStats();
  }

  get allReviews(): ReviewerResult[] {
    return this.reviews;
  }

  private get nAttempts(): number {
    return this.submissions.length;
  }

  private get nAccepted(): number {
    return this.reviews.filter((r) => typeof r.accept === 'number' && r.accept >= this.config.acceptScore).length;
  }

  private get totalStats(): InstanceStats {
    const submissionStats = this.submissions.reduce((acc, s) => acc.add(s.modelStats), new InstanceStats());
    return submissionStats.add(this.model.getStats());
  }

  async onSubmit(submission: ReviewSubmission): Promise<void> {
    this.submissions.push(submission);
    await this.review();
  }

  private async review(): Promise<number> {
    const review = await this.reviewer.review(this.problemStatement, this.submissions[this.submissions.length - 1]);
    this.reviews.push(review);

    const exitStatus = this.submissions[this.submissions.length - 1].info.exitStatus || '';
    if (exitStatus && exitStatus.toLowerCase().includes('exit_cost')) {
      this.nConsecExitCost++;
    } else {
      this.nConsecExitCost = 0;
    }

    return typeof review.accept === 'number' ? review.accept : 0;
  }

  retry(): boolean {
    const maxScore = Math.max(...this.reviews.map((r) => (typeof r.accept === 'number' ? r.accept : -100)), -100);
    const statStr = `n_samples=${this.nAttempts}, max_score=${maxScore}, n_accepted=${this.nAccepted}`;

    if (this.config.costLimit > 0 && this.totalStats.instanceCost > this.config.costLimit) {
      this.logger.info(
        `Exiting retry loop (${statStr}): Total attempt cost ` +
          `(${this.totalStats.instanceCost}) exceeds cost limit (${this.config.costLimit})`,
      );
      return false;
    }

    if (this.config.maxAttempts > 0 && this.nAttempts >= this.config.maxAttempts) {
      this.logger.info(`Exiting retry loop (${statStr}): max_attempts=${this.config.maxAttempts} reached`);
      return false;
    }

    if (this.config.maxAccepts > 0 && this.nAccepted >= this.config.maxAccepts) {
      this.logger.info(`Exiting retry loop (${statStr}): max_accepts=${this.config.maxAccepts} reached`);
      return false;
    }

    const remainingBudget = this.config.costLimit - this.totalStats.instanceCost;
    if (this.config.minBudgetForNewAttempt > 0 && remainingBudget < this.config.minBudgetForNewAttempt) {
      this.logger.info(
        `Exiting retry loop (${statStr}): Not enough budget left for a new attempt ` +
          `(${remainingBudget} remaining, ${this.config.minBudgetForNewAttempt} required)`,
      );
      return false;
    }

    return true;
  }

  getBest(): number | null {
    if (this.reviews.length === 0) {
      return null;
    }

    const scores = this.reviews.map((r) => (typeof r.accept === 'number' ? r.accept : -100));
    const maxScore = Math.max(...scores);

    // Find all indices with max score
    const maxIndices = scores
      .map((s, i) => ({ score: s, index: i }))
      .filter((item) => item.score === maxScore)
      .map((item) => item.index);

    // If multiple submissions have the same score, choose the one with fewest API calls
    const sortedMaxIndices = maxIndices.sort((a, b) => {
      const callsA = this.submissions[a].modelStats.apiCalls || Infinity;
      const callsB = this.submissions[b].modelStats.apiCalls || Infinity;
      return callsA - callsB;
    });

    const chosenIdx = sortedMaxIndices[0];
    this.logger.info(`Best submission: ${chosenIdx}`);

    return chosenIdx;
  }
}

/**
 * Factory `);
  }
}

} // namespace elizaos
