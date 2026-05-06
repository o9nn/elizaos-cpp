#pragma once
#include <algorithm>
#include <any>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>
#include "agents.hpp"
#include "exceptions.hpp"
#include "types.hpp"
#include "utils/log.hpp"
#include "utils/model-pricing.hpp"

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

using RetryConfig = z::infer<typeof RetryConfigSchema>;

/**
 * Generic API model configuration
 */

using GenericAPIModelConfig = z::infer<typeof GenericAPIModelConfigSchema>;

/**
 * Human model configuration
 */

using HumanModelConfig = z::infer<typeof HumanModelConfigSchema>;

/**
 * Replay model configuration
 */

using ReplayModelConfig = z::infer<typeof ReplayModelConfigSchema>;

/**
 * Instant empty submit model configuration
 */
struct InstantEmptySubmitModelConfig {
    std::string name;
    std::optional<double> delay;
};

/**
 * Human thought model configuration
 */
struct HumanThoughtModelConfig {
    std::string name;
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
public:
    void addCost(double cost);
    void updateTimestamp();
};

/**
 * Instance-specific statistics
 */
class InstanceStats {
public:
    InstanceStats add(InstanceStats other);
    InstanceStats subtract(InstanceStats other);
};

/**
 * Abstract base class for all models
 */
    void resetStats();
    InstanceStats getStats();
    void checkCostLimits();

/**
 * Human model for interactive input
 */
class HumanModel {
public:
    HumanModel(HumanModelConfig config, ToolConfig tools);
    void loadReadlineHistory();
    void updateStats();
    void handleRaiseCommands(const std::string& action);

private:
    bool catchEof_;
};

/**
 * LiteLLM model for API-based models
 */
class LiteLLMModel {
public:
    LiteLLMModel(GenericAPIModelConfig config, ToolConfig tools);
    std::vector<std::string> getApiKeys();
    std::string chooseApiKey();
    std::future<void> sleep();
    std::variant<Promise<ModelOutput, ModelOutput[]>> query(History history, std::optional<double> temperature, double n = 1);
    std::future<std::vector<ModelOutput>> singleQuery(const std::vector<HistoryItem>& messages, std::optional<double> n, std::optional<double> temperature);
    void updateStatsFromResponse(APIResponse response);
    double calculateCost(double inputTokens, double outputTokens);
    std::vector<HistoryItem> historyToMessages(History history);
    std::string getRole(HistoryItem historyItem);

private:
    std::vector<std::string> apiKeys_;
};

/**
 * Human model with thought prompting
 */
class HumanThoughtModel {
public:
};

/**
 * Replay model for replaying trajectories
 */
class ReplayModel {
public:
    ReplayModel(ReplayModelConfig config, ToolConfig tools);
    void nextReplay();
    std::future<ModelOutput> query(History _history);

private:
    std::variant<Array<Array<string, ModelOutput>>> replays_;
    bool usesFunctionCalling_;
    std::string submitCommand_;
};

/**
 * Instant empty submit model for testing
 */
class InstantEmptySubmitModel {
public:
    InstantEmptySubmitModel(std::optional<std::any> config, ToolConfig tools);
    std::future<ModelOutput> query(History _history);

private:
    double delay_;
};

/**
 * Predetermined test model for testing
 */
class PredeterminedTestModel {
public:
    PredeterminedTestModel(const std::variant<Array<string, ModelOutput>>& responses, std::optional<ToolConfig> tools);
    std::future<ModelOutput> query(History _history);

private:
    std::variant<Array<string, ModelOutput>> responses_;
};

// Export types for testing

/**
 * Factory function to create model instances
 */
AbstractModel getModel(ModelConfig config, ToolConfig tools);

} // namespace elizaos
