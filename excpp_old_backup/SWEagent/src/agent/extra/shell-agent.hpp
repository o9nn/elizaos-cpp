#include "..types.hpp"
#include ".agents.hpp"
#include ".history-processors.hpp"
#include ".models.hpp"
#include ".problem-statement.hpp"
#include ".tools/parsing.hpp"
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
 * Shell agent implementation with human-in-the-loop capability
 * Converted from sweagent/agent/extra/shell_agent.py
 */

/**
 * Shell agent that allows human intervention during execution
 */
class ShellAgent extends DefaultAgent {
  private originalModel: AbstractModel | null = null;
  private originalParser: AbstractParseFunction | null = null;

  constructor(config: {
    templates: TemplateConfig;
    tools: ToolHandler;
    historyProcessors: AbstractHistoryProcessor[];
    model: AbstractModel;
    maxRequeries?: number;
    name?: string;
  }) {
    super(config);
  }

    // Type check to ensure it's actually a ShellAgentConfig
    // Deep copy config to avoid shared state between different instances

  /**
   * Replace the current model with a HumanModel instance
   * This allows for human intervention during agent execution
   */

    // Set parser to ActionOnlyParser

  /**
   * Switch back to the original model from human mode
   * This is called when ^D is pressed in human mode
   */

  /**
   * Run the agent on a problem instance with human intervention capability
   */

    // Run action/observation loop

          // User pressed ^C, switch to human mode
          // Can only happen if we have a human model, so switch back

      // If robot is done but not in human mode, switch to human for submission

    // Get trajectory data for return

} // namespace elizaos
