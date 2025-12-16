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

  /**
   * Get action from the sampler
   */

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

    // Query model multiple times

    // Get colleague discussion

    // Create new messages with discussion

    // Get final completion

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

      // Add cache control to messages

    struct ComparisonLogEntry {
    [number, number] comparisonBetween;
    History messages;
    std::string response;
    double idx;
};

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
 * Factory function to create action sampler from configuration
 */
AbstractActionSampler createActionSampler(ActionSamplerConfig config, AbstractModel model, ToolHandler tools);

/**
 * Add get method to configs for compatibility
 */

} // namespace elizaos
