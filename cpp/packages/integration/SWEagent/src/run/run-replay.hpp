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

    // Load environment variables

    // Create deployment if provided

    // Set default output directory

    // Extract configuration from trajectory

    // Build config from trajectory data

    // Extract actions from trajectory

    // Save actions to file

    // Note: Cannot modify deployment on existing config
    // Deployment is handled separately if provided

    // Create a default environment config if not present

    // Create replay agent

    // Create default templates if not provided

    // Create tool handler with default config

    // Create model

    // Create actions file

    // Create environment and agent

    // Start environment

      // Get problem statement from trajectory
        // If it's already a problem statement instance, use it
          // Otherwise create from config

      // Run agent

      // Compare with original if available
      // Clean up environment

/**
 * Run from configuration
 */
std::future<void> runReplayFromConfig(RunReplayConfig config);

} // namespace elizaos
