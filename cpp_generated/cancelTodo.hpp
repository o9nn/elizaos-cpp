#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTONOMOUS-STARTER_SRC_PLUGIN-TODO_ACTIONS_CANCELTODO_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTONOMOUS-STARTER_SRC_PLUGIN-TODO_ACTIONS_CANCELTODO_H
#include "core.h"
#include "@elizaos/core.h"

class TaskCancellation;

class TaskCancellation : public object, public std::enable_shared_from_this<TaskCancellation> {
public:
    using std::enable_shared_from_this<TaskCancellation>::shared_from_this;
    string taskId;

    string taskName;

    boolean isFound;
};

extern string extractCancellationTemplate;
std::shared_ptr<Promise<std::shared_ptr<TaskCancellation>>> extractTaskCancellation(std::shared_ptr<IAgentRuntime> runtime, std::shared_ptr<Memory> message, array<std::shared_ptr<Task>> availableTasks, std::shared_ptr<State> state);

extern std::shared_ptr<Action> cancelTodoAction;
#endif
