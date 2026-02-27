#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_AUTONOMOUS_STARTER_SRC_PLUGIN_TODO_ACTIONS_CANCELTODO_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_AUTONOMOUS_STARTER_SRC_PLUGIN_TODO_ACTIONS_CANCELTODO_H
#include "core.hpp"
// External dependency removed

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
