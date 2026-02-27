#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_CORE_SRC_SPECS_V2_ROLES_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_CORE_SRC_SPECS_V2_ROLES_H
#include "core.hpp"
#include "../../roles.h"
using coreGetUserServerRole = getUserServerRole;
using coreFindWorldsForOwner = findWorldsForOwner;
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
