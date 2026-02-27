#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_CORE_SRC_ROLES_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_CORE_SRC_ROLES_H
#include "core.h"
#include "./entities.h"
#include "./logger.h"
#include "./types.h"

class ServerOwnershipState;

class ServerOwnershipState : public object, public std::enable_shared_from_this<ServerOwnershipState> {
public:
    using std::enable_shared_from_this<ServerOwnershipState>::shared_from_this;
    object servers;
};

std::shared_ptr<Promise<Role>> getUserServerRole(std::shared_ptr<IAgentRuntime> runtime, string entityId, string serverId);

std::shared_ptr<Promise<any>> findWorldsForOwner(std::shared_ptr<IAgentRuntime> runtime, string entityId);

#endif
