#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/core/src/specs/v2/roles.h"

std::shared_ptr<Promise<Role>> getUserServerRole(std::shared_ptr<IAgentRuntime> runtime, string entityId, string serverId)
{
    return coreGetUserServerRole(as<any>(runtime), entityId, serverId);
};


std::shared_ptr<Promise<any>> findWorldsForOwner(std::shared_ptr<IAgentRuntime> runtime, string entityId)
{
    return coreFindWorldsForOwner(as<any>(runtime), entityId);
};


