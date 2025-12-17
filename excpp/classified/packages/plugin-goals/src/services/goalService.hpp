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
#include "elizaos/core.hpp"
#include "schema.hpp"
#include "types.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Service for managing goal data
 */
class GoalDataManager {
public:
    GoalDataManager(IAgentRuntime runtime);
    std::variant<Promise<UUID, null>> createGoal(std::optional<std::any> params);
    std::future<std::vector<GoalData>> getGoals(std::optional<std::any> filters);
    std::variant<Promise<GoalData, null>> getGoal(UUID goalId);
    std::future<bool> updateGoal(UUID goalId, std::optional<std::any> updates);
    std::future<bool> deleteGoal(UUID goalId);
    std::future<std::vector<GoalData>> getUncompletedGoals(std::optional<std::string> ownerType, std::optional<UUID> ownerId);
    std::future<std::vector<GoalData>> getCompletedGoals(std::optional<std::string> ownerType, std::optional<UUID> ownerId);
    std::future<double> countGoals(const std::string& ownerType, UUID ownerId, std::optional<bool> isCompleted);
    std::future<std::vector<GoalData>> getAllGoalsForOwner(const std::string& ownerType, UUID ownerId);

private:
    IAgentRuntime runtime_;
};

/**
 * Creates a GoalDataManager instance
 * @deprecated Use runtime.getService('goals') instead in production code.
 * This function is kept for test mocking purposes only.
 * @param runtime - The agent runtime
 * @returns A new GoalDataManager instance
 */
GoalDataManager createGoalDataService(IAgentRuntime runtime);

/**
 * Service wrapper for the GoalService to be registered with the plugin
 */
class GoalService {
public:
    std::future<void> stop();
    static std::future<GoalService> start(IAgentRuntime runtime);
    std::optional<GoalDataManager> getDataManager();
    std::variant<Promise<UUID, null>> createGoal(std::optional<std::any> params);
    std::future<std::vector<GoalData>> getGoals(std::optional<std::any> filters);
    std::variant<Promise<GoalData, null>> getGoal(UUID goalId);
    std::future<bool> updateGoal(UUID goalId, std::optional<std::any> updates);
    std::future<bool> deleteGoal(UUID goalId);
    std::future<std::vector<GoalData>> getUncompletedGoals(std::optional<std::string> ownerType, std::optional<UUID> ownerId);
    std::future<std::vector<GoalData>> getCompletedGoals(std::optional<std::string> ownerType, std::optional<UUID> ownerId);
    std::future<double> countGoals(const std::string& ownerType, UUID ownerId, std::optional<bool> isCompleted);
    std::future<std::vector<GoalData>> getAllGoalsForOwner(const std::string& ownerType, UUID ownerId);

private:
    UUID agentId_;
    std::string ownerType_;
    UUID ownerId_;
    std::string name_;
};


} // namespace elizaos
