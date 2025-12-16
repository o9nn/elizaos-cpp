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

import type { IAgentRuntime, Memory, State } from '@elizaos/core';

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

  if (currentState.data?.workingMemory) {
    refreshedState.data.workingMemory = currentState.data.workingMemory;
  }

  return refreshedState;
}

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


  const updatedPlan = { ...state.data.actionPlan };
  const steps = [...updatedPlan.steps];

  if (stepIndex >= 0 && stepIndex < steps.length) {
    steps[stepIndex] = {
      ...steps[stepIndex],
      status,
      result,
      error,
    };

    updatedPlan.steps = steps;
    updatedPlan.currentStep = stepIndex + 1;

    return {
      ...state,
      data: {
        ...state.data,
        actionPlan: updatedPlan,
      },
    };
  }

  return state;
}

/**
 * Initializes or updates working memory in state
 *
 * @param state - Current state
 * @param key - Memory key
 * @param value - Memory value
 * @returns Updated state
 */
;

  return {
    ...state,
    data: {
      ...state.data,
      workingMemory: {
        ...workingMemory,
        [key]: {
          ...value,
          timestamp: Date.now(),
        },
      },
    },
  };
}


} // namespace elizaos
