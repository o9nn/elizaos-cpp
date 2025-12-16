#include ".agent/agents.hpp"
#include ".agent/problem-statement.hpp"
#include ".environment/swe-env.hpp"
#include ".types.hpp"
#include ".utils/config.hpp"
#include ".utils/log.hpp"
#include "common.hpp"
#include "hooks/types.hpp"
#include "types.hpp"
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
 * Run single instance
 * Converted from sweagent/run/run_single.py
 */

;
;
;
;
;
;
;
;
;
;
;

// Import the proper types from our new types module
;
{ RunSingleActionConfig, RunSingleConfig } from './types';

/**
 * Configuration schema using Zod
 */
const RunSingleConfigSchema = z.object({
  env: z.any().default({}),
  agent: z.any(),
  problemStatement: z.any().default(() => new EmptyProblemStatement()),
  outputDir: z.string().default('DEFAULT'),
  actions: z
    .object({
      openPr: z.boolean().default(false),
      prConfig: z.any().optional(),
      applyPatchLocally: z.boolean().default(false),
    })
    .default({}),
  envVarPath: z.string().optional(),
});

/**
 * Get default output directory
 */


  // Generate default based on problem statement and agent
  const timestamp = new Date().toISOString().replace(/[:.]/g, '-');
  const psId = problemStatement.id || 'unknown';
  const agentName = agent.type || 'agent';

  return path.join('trajectories', `${psId}_${agentName}_${timestamp}`);
}

/**
 * Run single instance
 */
class RunSingle {
  private env: SWEEnv;
  private agent: AbstractAgent;
  private problemStatement: ProblemStatement | ProblemStatementConfig;
  private outputDir: string;
  private hooks: RunHook[];
  private actions: RunSingleActionConfig;
  private logger: AgentLogger;

  constructor(config: {
    env: SWEEnv;
    agent: AbstractAgent;
    problemStatement: ProblemStatement | ProblemStatementConfig;
    outputDir?: string;
    hooks?: RunHook[];
    actions?: RunSingleActionConfig;
  }) {
    this.env = config.env;
    this.agent = config.agent;
    this.problemStatement = config.problemStatement;
    this.outputDir = config.outputDir || '.';
    this.hooks = config.hooks || [];
    this.actions = config.actions || {
      openPr: false,
      applyPatchLocally: false,
    };
    this.logger = getLogger('run-single', '🏃');

    // RunHook and AbstractAgentHook are different interfaces
    // We don't add RunHooks to the agent, they're managed separately
  }

  static async fromConfig(config: RunSingleConfig): Promise<RunSingle> {
    // Load environment variables
    if (config.envVarPath) {
      loadEnvironmentVariables(config.envVarPath);
    }

    // Create environment
    const env = SWEEnv.fromConfig(config.env);

    // Create agent
    const agent = await getAgentFromConfig(config.agent);

    // Set default output directory
    const outputDir = getDefaultOutputDir(config.outputDir, config.problemStatement as ProblemStatement, config.agent);

    return new RunSingle({
      env,
      agent,
      problemStatement: config.problemStatement,
      outputDir,
      actions: config.actions,
    });
  }

  addHook(hook: RunHook): void {
    this.hooks.push(hook);
    // RunHook and AbstractAgentHook are different interfaces
    // RunHooks are managed separately from agent hooks
  }

  async run(): Promise<AgentRunResult> {
    this.logger.info('Starting run');

    // Create output directory
    fs.mkdirSync(this.outputDir, { recursive: true });

    // Start environment
    await this.env.start();

    try {
      // Run hooks before start
      for (const hook of this.hooks) {
        if (hook.onStart) {
          await hook.onStart();
        }
      }

      // Run agent
      const result = await this.agent.run(
        this.env as unknown as AgentEnvironment,
        this.problemStatement,
        this.outputDir,
      );

      // Save predictions if we have a promising patch
      if (isPromisingPatch(result.info)) {
        const instanceId =
          typeof this.problemStatement === 'object' && 'id' in this.problemStatement
            ? this.problemStatement.id
            : 'unknown';
        savePredictions(this.outputDir, instanceId, result);
      }

      // Run hooks after completion
      for (const hook of this.hooks) {
        if (hook.onInstanceCompleted) {
          await hook.onInstanceCompleted({ result });
        }
      }

      // Apply actions if configured
      if (this.actions.applyPatchLocally && result.info.submission) {
        await this.applyPatchLocally(result.info.submission);
      }

      if (this.actions.openPr && result.info.submission) {
        await this.openPullRequest(result.info.submission);
      }

      this.logger.info('Run completed successfully');
      return result;
    } finally {
      // Clean up environment
      await this.env.close();
    }
  }

  private async applyPatchLocally(patch: string): Promise<void> {
    this.logger.info('Applying patch locally');

    // Save patch to file
    const patchPath = path.join(this.outputDir, 'model.patch');
    fs.writeFileSync(patchPath, patch);

    // Apply using git apply (would need actual implementation)
    this.logger.info(`Patch saved to ${patchPath}`);
  }

  private async openPullRequest(_patch: string): Promise<void> {
    this.logger.info('Opening pull request');
    // This would need actual GitHub API implementation
    this.logger.warning('Pull request opening not yet implemented');
  }
}

/**
 * Run from configuration
 */
std::future<AgentRunResult> runFromConfig(RunSingleConfig config);

} // namespace elizaos
