#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTONOMOUS-STARTER_SRC_PLUGIN-TODO_SERVICES_TASKSERVICE_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTONOMOUS-STARTER_SRC_PLUGIN-TODO_SERVICES_TASKSERVICE_H
#include "core.h"
#include "@elizaos/core.h"
#include "./dbCompatibility.h"

class TaskService;

class TaskService : public object, public std::enable_shared_from_this<TaskService> {
public:
    using std::enable_shared_from_this<TaskService>::shared_from_this;
    std::shared_ptr<IAgentRuntime> runtime;

    TaskService(std::shared_ptr<IAgentRuntime> runtime_);
    virtual std::shared_ptr<Promise<array<std::shared_ptr<Task>>>> getTasks(object params = undefined);
    virtual std::shared_ptr<Promise<any>> createTask(object params);
    virtual std::shared_ptr<Promise<boolean>> updateTask(std::shared_ptr<UUID> taskId, object updates);
    virtual std::shared_ptr<Task> normalizeTask(std::shared_ptr<Task> task);
    virtual std::shared_ptr<Promise<array<std::shared_ptr<Task>>>> searchTasksByName(string name, object params = undefined);
};

std::shared_ptr<TaskService> createTaskService(std::shared_ptr<IAgentRuntime> runtime);

#endif
