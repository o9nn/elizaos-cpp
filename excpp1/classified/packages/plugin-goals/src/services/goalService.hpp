#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include ".schema.hpp"
#include ".types.hpp"
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Service for managing goal data
 */
class GoalDataManager {
public:
    GoalDataManager(IAgentRuntime runtime);
    std::variant<Promise<UUID, null>> createGoal(std::optional<std::variant<{
    agentId: UUID;
    ownerType: 'agent', 'entity';
    ownerId: UUID;
    name: string;
    description: string;
    metadata: Record<string>> params, std::optional<string[];
  }> any>;
    std::future<bool> deleteGoal(UUID goalId);
    std::future<double> countGoals(const std::variant<'agent', 'entity'>& ownerType, UUID ownerId, std::optional<bool> isCompleted);

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
    std::future<GoalService> start(IAgentRuntime runtime);
    std::optional<GoalDataManager> getDataManager();
    std::variant<Promise<UUID, null>> createGoal(std::optional<std::variant<{
    agentId: UUID;
    ownerType: 'agent', 'entity';
    ownerId: UUID;
    name: string;
    description: string;
    metadata: Record<string>> params, std::optional<string[];
  }> any>;
    std::future<bool> deleteGoal(UUID goalId);
    std::future<double> countGoals(const std::variant<'agent', 'entity'>& ownerType, UUID ownerId, std::optional<bool> isCompleted);


} // namespace elizaos
