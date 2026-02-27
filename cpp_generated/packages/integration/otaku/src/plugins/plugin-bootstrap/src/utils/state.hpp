#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_OTAKU_SRC_PLUGINS_PLUGIN-BOOTSTRAP_SRC_UTILS_STATE_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_OTAKU_SRC_PLUGINS_PLUGIN-BOOTSTRAP_SRC_UTILS_STATE_H
#include "core.h"
#include "@elizaos/core.h"

std::shared_ptr<Promise<std::shared_ptr<State>>> refreshStateAfterAction(std::shared_ptr<IAgentRuntime> runtime, std::shared_ptr<Memory> message, std::shared_ptr<State> currentState, array<any> actionResults);

template <typename P2>
std::shared_ptr<State> updateActionPlanStep(std::shared_ptr<State> state, double stepIndex, P2 status, Record<string, any> result = undefined, string error = undefined);

std::shared_ptr<State> updateWorkingMemory(std::shared_ptr<State> state, string key, Record<string, any> value);

template <typename P2>
std::shared_ptr<State> updateActionPlanStep(std::shared_ptr<State> state, double stepIndex, P2 status, Record<string, any> result, string error)
{
    if (!state->data->actionPlan) {
        return state;
    }
    auto updatedPlan = utils::assign(object{
    }, state->data->actionPlan);
    auto steps = array<any>{ updatedPlan["steps"] };
    if (AND((stepIndex >= 0), (stepIndex < steps->get_length()))) {
        steps[stepIndex] = utils::assign(object{
            , 
            object::pair{std::string("status"), std::string("status")}, 
            object::pair{std::string("result"), std::string("result")}, 
            object::pair{std::string("error"), std::string("error")}
        }, const_(steps)[stepIndex]);
        updatedPlan["steps"] = steps;
        updatedPlan["currentStep"] = stepIndex + 1;
        return utils::assign(object{
            , 
            object::pair{std::string("data"), utils::assign(object{
                , 
                object::pair{std::string("actionPlan"), updatedPlan}
            }, state->data)}
        }, state);
    }
    return state;
};


#endif
