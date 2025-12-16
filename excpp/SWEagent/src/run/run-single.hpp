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

// Import the proper types from our new types module

/**
 * Configuration schema using Zod
 */

/**
 * Get default output directory
 */
std::string getDefaultOutputDir(const std::string& outputDir, ProblemStatement problemStatement, AgentConfig agent);

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

    // Load environment variables

    // Create environment

    // Create agent

    // Set default output directory

    // RunHook and AbstractAgentHook are different interfaces
    // RunHooks are managed separately from agent hooks

    // Create output directory

    // Start environment

      // Run hooks before start

      // Run agent

      // Save predictions if we have a promising patch

      // Run hooks after completion

      // Apply actions if configured

      // Clean up environment

    // Save patch to file

    // Apply using git apply (would need actual implementation)

    // This would need actual GitHub API implementation

/**
 * Run from configuration
 */
std::future<AgentRunResult> runFromConfig(RunSingleConfig config);

} // namespace elizaos
