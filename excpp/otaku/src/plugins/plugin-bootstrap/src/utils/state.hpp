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
 * Refreshes state after action execution to keep prompts and action results in sync
 * This ensures subsequent actions have access to the latest wallet balances and action results
 *
 * @param runtime - Agent runtime
 * @param message - Current message being processed
 * @param currentState - Current state object
 * @param actionResults - Array of action results from this execution round
 * @returns Updated state with fresh data
 */
std::future<State> refreshStateAfterAction(IAgentRuntime runtime, Memory message, State currentState, const std::vector<unknown>& actionResults);

/**
 * Updates action plan state with step completion status
 *
 * @param state - Current state
 * @param stepIndex - Index of the step to update
 * @param status - New status (completed, failed, pending)
 * @param result - Optional result data
 * @param error - Optional error message
 * @returns Updated state
 */
State updateActionPlanStep(State state, double stepIndex, 'completed' | 'failed' | 'pending' status, std::optional<Record<string> result, auto unknown>, std::optional<std::string> error);

/**
 * Initializes or updates working memory in state
 *
 * @param state - Current state
 * @param key - Memory key
 * @param value - Memory value
 * @returns Updated state
 */
State updateWorkingMemory(State state, const std::string& key, Record<string value, auto unknown>);


} // namespace elizaos
