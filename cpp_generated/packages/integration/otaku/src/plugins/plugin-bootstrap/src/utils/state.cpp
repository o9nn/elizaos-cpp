#include "/home/runner/work/elizaos-cpp/elizaos-cpp/otaku/src/plugins/plugin-bootstrap/src/utils/state.h"

std::shared_ptr<Promise<std::shared_ptr<State>>> refreshStateAfterAction(std::shared_ptr<IAgentRuntime> runtime, std::shared_ptr<Memory> message, std::shared_ptr<State> currentState, array<any> actionResults)
{
    auto refreshedState = std::async([=]() { runtime->composeState(message, array<string>{ std::string("RECENT_MESSAGES"), std::string("ACTION_STATE"), std::string("WALLET_STATE") }); });
    refreshedState->data->actionResults = actionResults;
    if (currentState->data->actionPlan) {
        refreshedState->data->actionPlan = currentState->data->actionPlan;
    }
    if (currentState->data->workingMemory) {
        refreshedState->data->workingMemory = currentState->data->workingMemory;
    }
    return refreshedState;
};


std::shared_ptr<State> updateWorkingMemory(std::shared_ptr<State> state, string key, Record<string, any> value)
{
    auto workingMemory = OR((state->data->workingMemory), (object{}));
    return utils::assign(object{
        , 
        object::pair{std::string("data"), utils::assign(object{
            , 
            object::pair{std::string("workingMemory"), utils::assign(object{
                , 
                object::pair{key, utils::assign(object{
                    , 
                    object::pair{std::string("timestamp"), Date->now()}
                }, value)}
            }, workingMemory)}
        }, state->data)}
    }, state);
};


