#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-GOALS_SRC_ACTIONS_CANCELGOAL_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-GOALS_SRC_ACTIONS_CANCELGOAL_H
#include "core.h"
#include "@elizaos/core.h"
#include "../services/goalService.h"
#include "../types.h"

class TaskCancellation;

class TaskCancellation : public object, public std::enable_shared_from_this<TaskCancellation> {
public:
    using std::enable_shared_from_this<TaskCancellation>::shared_from_this;
    string taskId;

    string taskName;

    boolean isFound;
};

extern string extractCancellationTemplate;
std::shared_ptr<Promise<std::shared_ptr<TaskCancellation>>> extractTaskCancellation(std::shared_ptr<IAgentRuntime> runtime, std::shared_ptr<Memory> message, array<std::shared_ptr<GoalData>> availableGoals, std::shared_ptr<State> state);

extern std::shared_ptr<Action> cancelGoalAction;
#endif
