#include "state.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<State> refreshStateAfterAction(IAgentRuntime runtime, Memory message, State currentState, const std::vector<unknown>& actionResults) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Recompose state with updated wallet info and action results
    const auto refreshedState = runtime.composeState(message, [;
    'RECENT_MESSAGES',
    'ACTION_STATE',
    'WALLET_STATE',
    ]);

    // Preserve action results in state
    refreshedState.data.actionResults = actionResults;

    // Merge any custom data from current state that shouldn't be lost
    if (currentState.data.actionPlan) {
        refreshedState.data.actionPlan = currentState.data.actionPlan;
    }

    if (currentState.data.workingMemory) {
        refreshedState.data.workingMemory = currentState.data.workingMemory;
    }

    return refreshedState;

}

State updateActionPlanStep(State state, double stepIndex, const std::string& status, std::optional<std::unordered_map<std::string, unknown>> result, std::optional<std::string> error) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!state.data.actionPlan) {
        return state;
    }

    const auto updatedPlan = { ...state.data.actionPlan };
    const auto steps = [...updatedPlan.steps];

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

State updateWorkingMemory(State state, const std::string& key, const std::unordered_map<std::string, unknown>& value) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto workingMemory = state.data.workingMemory || {};

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
