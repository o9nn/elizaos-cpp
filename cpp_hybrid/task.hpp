#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CORE_SRC_TYPES_TASK_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CORE_SRC_TYPES_TASK_H
#include "core.h"
#include "./memory.h"
#include "./primitives.h"
#include "./runtime.h"
#include "./state.h"

typedef object TaskMetadata;

class TaskWorker;
class Task;

class TaskWorker : public object, public std::enable_shared_from_this<TaskWorker> {
public:
    using std::enable_shared_from_this<TaskWorker>::shared_from_this;
    string name;

    std::function<std::shared_ptr<Promise<void>>(std::shared_ptr<IAgentRuntime>, object, std::shared_ptr<Task>)> execute;

    std::function<std::shared_ptr<Promise<boolean>>(std::shared_ptr<IAgentRuntime>, std::shared_ptr<Memory>, std::shared_ptr<State>)> validate;
};

class Task : public object, public std::enable_shared_from_this<Task> {
public:
    using std::enable_shared_from_this<Task>::shared_from_this;
    std::shared_ptr<UUID> id;

    string name;

    double updatedAt;

    TaskMetadata metadata;

    string description;

    std::shared_ptr<UUID> roomId;

    std::shared_ptr<UUID> worldId;

    std::shared_ptr<UUID> entityId;

    array<string> tags;
};

#endif
