#pragma once
#include <any>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include ".types.hpp"
#include ".utils/log.hpp"
#include "agents.hpp"
#include "models.hpp"
#include "problem-statement.hpp"
#include "types.hpp"
#include "utils/template.hpp"

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
};

/**
 * Abstract base class for action samplers
 */

/**
 * Configuration for AskColleagues sampler
 */
struct AskColleaguesConfig {
    std::string type;
    double nSamples;
};

/**
 * AskColleagues action sampler
 * Queries multiple model instances and synthesizes their responses
 */
class AskColleagues {
public:
    AskColleagues(AskColleaguesConfig config, AbstractModel model, ToolHandler tools);
    static AskColleagues fromConfig(AskColleaguesConfig config, AbstractModel model, ToolHandler tools);
    std::string getColleagueDiscussion(const std::vector<ModelOutput>& completions);
    std::future<ActionSamplerOutput> getAction(ProblemStatement _problemStatement, Trajectory _trajectory, History history);

private:
    AskColleaguesConfig config_;
};

/**
 * Configuration for BinaryTrajectoryComparison sampler
 */
struct BinaryTrajectoryComparisonConfig {
    std::string type;
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
class BinaryTrajectoryComparison {
public:
    BinaryTrajectoryComparison(BinaryTrajectoryComparisonConfig config, AbstractModel model, ToolHandler tools);
    static BinaryTrajectoryComparison fromConfig(BinaryTrajectoryComparisonConfig config, AbstractModel model, ToolHandler tools);
    std::string formatTrajectory(Trajectory trajectory);
    History formatMessages(std::optional<std::any> params);
    std::vector<ModelOutput> filterDuplicates(const std::vector<ModelOutput>& completions);
    std::vector<ModelOutput> filterParseableCompletions(const std::vector<ModelOutput>& completions);
    bool containsEdits(const std::vector<ModelOutput>& completions);
    std::future<std::vector<ModelOutput>> getCompletions(History history);
    std::variant<0, 1> interpret(const std::string& response);
    std::future<ActionSamplerOutput> getAction(ProblemStatement _problemStatement, Trajectory _trajectory, History history);

private:
    BinaryTrajectoryComparisonConfig config_;
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
using ActionSamplerConfig = std::variant<, (BinaryTrajectoryComparisonConfig, (AskColleaguesConfig>;

/**
 * Factory function to create action sampler from configuration
 */
AbstractActionSampler createActionSampler(ActionSamplerConfig config, AbstractModel model, ToolHandler tools);

/**
 * Add get method to configs for compatibility
 */

} // namespace elizaos
