#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-TODO_SRC_ACTIONS_COMPLETETODO_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-TODO_SRC_ACTIONS_COMPLETETODO_H
#include "core.h"
#include "@elizaos/core.h"
#include "../services/todoService.h"

class TaskCompletion;

class TaskCompletion : public object, public std::enable_shared_from_this<TaskCompletion> {
public:
    using std::enable_shared_from_this<TaskCompletion>::shared_from_this;
    string taskId;

    string taskName;

    boolean isFound;
};

extern string extractCompletionTemplate;
std::shared_ptr<Promise<std::shared_ptr<TaskCompletion>>> extractTaskCompletion(std::shared_ptr<IAgentRuntime> runtime, std::shared_ptr<Memory> message, array<std::shared_ptr<TodoData>> availableTasks, std::shared_ptr<State> state);

extern std::shared_ptr<Action> completeTodoAction;
#endif
