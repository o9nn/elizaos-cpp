#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-GOALS_SRC_ACTIONS_UPDATEGOAL_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-GOALS_SRC_ACTIONS_UPDATEGOAL_H
#include "core.h"
#include "@elizaos/core.h"
#include "../services/goalService.h"
#include "../types.h"

class GoalSelection;
class GoalUpdate;

class GoalSelection : public object, public std::enable_shared_from_this<GoalSelection> {
public:
    using std::enable_shared_from_this<GoalSelection>::shared_from_this;
    string goalId;

    string goalName;

    boolean isFound;
};

class GoalUpdate : public object, public std::enable_shared_from_this<GoalUpdate> {
public:
    using std::enable_shared_from_this<GoalUpdate>::shared_from_this;
    string name;

    string description;
};

extern string extractGoalTemplate;
extern string extractUpdateTemplate;
std::shared_ptr<Promise<std::shared_ptr<GoalSelection>>> extractGoalSelection(std::shared_ptr<IAgentRuntime> runtime, std::shared_ptr<Memory> message, array<std::shared_ptr<GoalData>> availableGoals);

std::shared_ptr<Promise<any>> extractGoalUpdate(std::shared_ptr<IAgentRuntime> runtime, std::shared_ptr<Memory> message, std::shared_ptr<GoalData> goal);

extern std::shared_ptr<Action> updateGoalAction;
#endif
