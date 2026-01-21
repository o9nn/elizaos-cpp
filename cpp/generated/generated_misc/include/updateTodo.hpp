#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTONOMOUS-STARTER_SRC_PLUGIN-TODO_ACTIONS_UPDATETODO_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTONOMOUS-STARTER_SRC_PLUGIN-TODO_ACTIONS_UPDATETODO_H
#include "core.h"
#include "@elizaos/core.h"

class TaskSelection;
class TaskUpdate;

class TaskSelection : public object, public std::enable_shared_from_this<TaskSelection> {
public:
    using std::enable_shared_from_this<TaskSelection>::shared_from_this;
    string taskId;

    string taskName;

    boolean isFound;
};

class TaskUpdate : public object, public std::enable_shared_from_this<TaskUpdate> {
public:
    using std::enable_shared_from_this<TaskUpdate>::shared_from_this;
    string name;

    string description;

    any priority;

    boolean urgent;

    any dueDate;

    any recurring;
};

extern string extractTaskTemplate;
extern string extractUpdateTemplate;
std::shared_ptr<Promise<std::shared_ptr<TaskSelection>>> extractTaskSelection(std::shared_ptr<IAgentRuntime> runtime, std::shared_ptr<Memory> message, array<std::shared_ptr<Task>> availableTasks);

std::shared_ptr<Promise<any>> extractTaskUpdate(std::shared_ptr<IAgentRuntime> runtime, std::shared_ptr<Memory> message, std::shared_ptr<Task> task);

std::shared_ptr<Promise<std::shared_ptr<Task>>> applyTaskUpdate(std::shared_ptr<IAgentRuntime> runtime, std::shared_ptr<Task> task, std::shared_ptr<TaskUpdate> update);

extern std::shared_ptr<Action> updateTodoAction;
#endif
