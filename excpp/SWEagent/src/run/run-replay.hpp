#include ".agent/agents.hpp"
#include ".agent/models.hpp"
#include ".agent/problem-statement.hpp"
#include ".environment/deployment.hpp"
#include ".environment/swe-env.hpp"
#include ".utils/config.hpp"
#include ".utils/log.hpp"
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
 * Replay agent trajectory
 * Converted from sweagent/run/run_replay.py
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
;

/**
 * Run replay configuration
 */
struct RunReplayConfig {
    std::string trajPath;
    std::optional<DeploymentConfig> deployment;
    std::optional<std::string> outputDir;
    std::optional<std::string> envVarPath;
    std::optional<std::vector<std::string>> updateConfig;
};


/**
 * Configuration schema
 */
const RunReplayConfigSchema = z.object({
  trajPath: z.string(),
  deployment: z.custom<DeploymentConfig>().optional(),
  outputDir: z.string().default('DEFAULT'),
  envVarPath: z.string().optional(),
  updateConfig: z.array(z.string()).default([]),
});

/**
 * Run replay - replay an agent trajectory
 */
class RunReplay {
  private trajPath: string;
  // private deployment?: AbstractDeployment;  // Not used currently
  private outputDir: string;
  private catchErrors: boolean;
  private requireZeroExitCode: boolean;
  private logger: AgentLogger;
  private trajData: TrajectoryData;

  constructor(config: {
    trajPath: string;
    deployment?: AbstractDeployment;
    outputDir?: string;
    updateConfig?: string[];
    catchErrors?: boolean;
    requireZeroExitCode?: boolean;
  }) {
    this.trajPath = config.trajPath;
    // this.deployment = config.deployment;  // Not used currently
    this.outputDir = config.outputDir || '.';
    // this._updateConfig = (config as any).updateConfig || [];  // Not implemented yet
    this.catchErrors = config.catchErrors !== false;
    this.requireZeroExitCode = config.requireZeroExitCode || false;
    this.logger = getLogger('run-replay', '🔄');

    // Load trajectory data
    const trajContent = fs.readFileSync(this.trajPath, 'utf-8');
    this.trajData = JSON.parse(trajContent);
  }

  static fromConfig(config: RunReplayConfig): RunReplay {
    // Load environment variables
    if (config.envVarPath) {
      loadEnvironmentVariables(config.envVarPath);
    }

    // Create deployment if provided
    const deployment = config.deployment ? getDeployment(config.deployment) : undefined;

    // Set default output directory
    if (config.outputDir === 'DEFAULT') {
      const timestamp = new Date().toISOString().replace(/[:.]/g, '-');
      config.outputDir = path.join('replays', `replay_${timestamp}`);
    }

    return new RunReplay({
      trajPath: config.trajPath,
      deployment,
      outputDir: config.outputDir,
      updateConfig: config.updateConfig,
    });
  }

  get instanceId(): string {
    return this.trajData.info?.instance_id || path.basename(this.trajPath, '.traj');
  }

  private getConfigFromAgent(): Partial<RunSingleConfig> {
    // Extract configuration from trajectory
    if (this.trajData.replay_config) {
      return JSON.parse(this.trajData.replay_config as string);
    }

    // Build config from trajectory data
    const config: Partial<RunSingleConfig> = {};

    if (this.trajData.environment) {
      config.env = this.trajData.environment as any;
    }

    config.agent = {
      type: 'default' as const,
      name: 'replay-agent',
      model: {
        name: 'replay' as const,
        replayPath: this.trajPath,
      } as any,
      templates: {} as any,
      tools: {} as any,
      historyProcessors: [],
      maxRequeries: 0,
    } as DefaultAgentConfig;

    if (this.trajData.problemStatement) {
      config.problemStatement = this.trajData.problemStatement;
    }

    return config;
  }

  private createActionsFile(): void {
    // Extract actions from trajectory
    const actions: string[] = [];

    for (const step of this.trajData.trajectory || []) {
      if (step.action) {
        actions.push(step.action);
      }
    }

    // Save actions to file
    const actionsPath = path.join(this.outputDir, 'actions.txt');
    fs.mkdirSync(this.outputDir, { recursive: true });
    fs.writeFileSync(actionsPath, actions.join('\n'));

    this.logger.info(`Actions saved to ${actionsPath}`);
  }

  private async getEnv(): Promise<SWEEnv> {
    const config = this.getConfigFromAgent();

    // Note: Cannot modify deployment on existing config
    // Deployment is handled separately if provided

    // Create a default environment config if not present
    const envConfig = config.env || {
      name: 'default',
      deployment: {
        type: 'docker' as const,
        image: 'python:3.11',
        pythonStandaloneDir: '',
        volumes: {},
        environment: {},
        removeOnStop: true,
        workDir: '/workspace',
      },
      postStartupCommands: [],
      postStartupCommandTimeout: 300,
    };

    return SWEEnv.fromConfig(envConfig);
  }

  private async getAgent(): Promise<DefaultAgent> {
    const config = this.getConfigFromAgent();

    // Create replay agent
    const agentConfig = config.agent && config.agent.type === 'default' ? config.agent : null;

    // Create default templates if not provided
    const templates =
      agentConfig?.templates ||
      ({
        systemTemplate: '',
        instanceTemplate: '',
        nextStepTemplate: '',
        nextStepTruncatedObservationTemplate: '',
        formatErrorTemplate: '',
        demonstrationTemplate: '',
        demonstrationObservationTemplate: '',
        historyToMessagesPrompt: '',
        historyTruncatedTemplate: '',
        hypothesisTemplate: '',
        hypothesisObservationTemplate: '',
      } as any);

    // Create tool handler with default config
    const toolConfig: ToolConfig = agentConfig?.tools || {
      commands: [],
      executionTimeout: 500,
      maxConsecutiveExecutionTimeouts: 3,
      totalExecutionTimeout: 7200,
      submitCommand: 'submit',
      useFunctionCalling: false,
      formatErrorTemplate: 'Invalid format',
    };
    const toolHandler = new ToolHandler(toolConfig);

    // Create model
    const model = getModel(
      {
        name: 'replay',
        replayPath: this.trajPath,
      },
      toolConfig,
    );

    return new DefaultAgent({
      templates,
      tools: toolHandler,
      historyProcessors: agentConfig?.historyProcessors || [],
      model,
      catchErrors: this.catchErrors,
      alwaysRequireZeroExitCode: this.requireZeroExitCode,
    });
  }

  async main(): Promise<void> {
    this.logger.info(`Replaying trajectory from ${this.trajPath}`);

    // Create actions file
    this.createActionsFile();

    // Create environment and agent
    const env = await this.getEnv();
    const agent = await this.getAgent();

    // Start environment
    await env.start();

    try {
      // Get problem statement from trajectory
      let problemStatement;
      if (this.trajData.problemStatement) {
        // If it's already a problem statement instance, use it
        if ('getProblemStatement' in this.trajData.problemStatement) {
          problemStatement = this.trajData.problemStatement;
        } else {
          // Otherwise create from config
          problemStatement = new TextProblemStatement({
            text: (this.trajData.problemStatement as any).text || 'Replay of previous run',
            id: this.instanceId,
          });
        }
      } else {
        problemStatement = new TextProblemStatement({
          text: 'Replay of previous run',
          id: this.instanceId,
        });
      }

      // Run agent
      const result = await agent.run(env, problemStatement, this.outputDir);

      this.logger.info('Replay completed');
      this.logger.info(`Exit status: ${result.info.exitStatus}`);

      // Compare with original if available
      if (this.trajData.info?.submission && result.info.submission) {
        if (this.trajData.info.submission === result.info.submission) {
          this.logger.info('✅ Replay produced identical submission');
        } else {
          this.logger.warning('⚠️ Replay produced different submission');
        }
      }
    } finally {
      // Clean up environment
      await env.close();
    }
  }
}

/**
 * Run from configuration
 */
std::future<void> runReplayFromConfig(RunReplayConfig config);

} // namespace elizaos
