#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-TODO_SRC_TYPES_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-TODO_SRC_TYPES_H
#include "core.h"
#include "@elizaos/core.h"

typedef Record<string, any> TodoMetadata;

namespace @elizaos/core {
    class ServiceTypeRegistry;
}
class TodoData;
class CreateTodoInput;
class UpdateTodoInput;

namespace std:("@elizaos/core") {
    class ServiceTypeRegistry : public object, public std::enable_shared_from_this<ServiceTypeRegistry> {
    public:
        using std::enable_shared_from_this<ServiceTypeRegistry>::shared_from_this;
        string TODO;

        string TODO_REMINDER;
    };

}
extern object TodoServiceType, satisfies, Partial;
class TodoData : public object, public std::enable_shared_from_this<TodoData> {
public:
    using std::enable_shared_from_this<TodoData>::shared_from_this;
    std::shared_ptr<UUID> id;

    std::shared_ptr<UUID> agentId;

    std::shared_ptr<UUID> worldId;

    std::shared_ptr<UUID> roomId;

    std::shared_ptr<UUID> entityId;

    string name;

    string description;

    any type;

    double priority;

    boolean isUrgent;

    boolean isCompleted;

    std::shared_ptr<Date> dueDate;

    std::shared_ptr<Date> completedAt;

    std::shared_ptr<Date> createdAt;

    std::shared_ptr<Date> updatedAt;

    TodoMetadata metadata;

    array<string> tags;
};

class CreateTodoInput : public object, public std::enable_shared_from_this<CreateTodoInput> {
public:
    using std::enable_shared_from_this<CreateTodoInput>::shared_from_this;
    std::shared_ptr<UUID> agentId;

    std::shared_ptr<UUID> worldId;

    std::shared_ptr<UUID> roomId;

    std::shared_ptr<UUID> entityId;

    string name;

    string description;

    any type;

    double priority;

    boolean isUrgent;

    std::shared_ptr<Date> dueDate;

    TodoMetadata metadata;

    array<string> tags;
};

class UpdateTodoInput : public object, public std::enable_shared_from_this<UpdateTodoInput> {
public:
    using std::enable_shared_from_this<UpdateTodoInput>::shared_from_this;
    string name;

    string description;

    any type;

    double priority;

    boolean isUrgent;

    boolean isCompleted;

    std::shared_ptr<Date> dueDate;

    std::shared_ptr<Date> completedAt;

    TodoMetadata metadata;
};

namespace @elizaos/core {
}
#endif
