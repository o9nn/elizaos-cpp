#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-GOALS_SRC_TYPES_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-GOALS_SRC_TYPES_H
#include "core.h"
#include "@elizaos/core.h"

namespace @elizaos/core {
    class ServiceTypeRegistry;
}
class GoalData;

namespace std:("@elizaos/core") {
    class ServiceTypeRegistry : public object, public std::enable_shared_from_this<ServiceTypeRegistry> {
    public:
        using std::enable_shared_from_this<ServiceTypeRegistry>::shared_from_this;
        string GOALS;
    };

}
extern object GoalServiceType, satisfies, Partial;
class GoalData : public object, public std::enable_shared_from_this<GoalData> {
public:
    using std::enable_shared_from_this<GoalData>::shared_from_this;
    std::shared_ptr<UUID> id;

    std::shared_ptr<UUID> agentId;

    any ownerType;

    std::shared_ptr<UUID> ownerId;

    string name;

    any description;

    boolean isCompleted;

    any completedAt;

    std::shared_ptr<Date> createdAt;

    std::shared_ptr<Date> updatedAt;

    Record<string, any> metadata;

    array<string> tags;
};

namespace @elizaos/core {
}
#endif
