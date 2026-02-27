#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-GOALS_SRC_ACTIONS_CONFIRMGOAL_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-GOALS_SRC_ACTIONS_CONFIRMGOAL_H
#include "core.h"
#include "@elizaos/core.h"
#include "../services/goalService.h"

class PendingGoalData;
class ConfirmationResponse;

class PendingGoalData : public object, public std::enable_shared_from_this<PendingGoalData> {
public:
    using std::enable_shared_from_this<PendingGoalData>::shared_from_this;
    string name;

    string description;

    any taskType;

    any priority;

    boolean urgent;

    string dueDate;

    any recurring;

    array<string> tags;

    Record<string, any> metadata;
};

class ConfirmationResponse : public object, public std::enable_shared_from_this<ConfirmationResponse> {
public:
    using std::enable_shared_from_this<ConfirmationResponse>::shared_from_this;
    boolean isConfirmation;

    boolean shouldProceed;

    string modifications;
};

extern string extractConfirmationTemplate;
std::shared_ptr<Promise<std::shared_ptr<ConfirmationResponse>>> extractConfirmationIntent(std::shared_ptr<IAgentRuntime> runtime, std::shared_ptr<Memory> message, any pendingTask, std::shared_ptr<State> state);

extern std::shared_ptr<Action> confirmGoalAction;
#endif
