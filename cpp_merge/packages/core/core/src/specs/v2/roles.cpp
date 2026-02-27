#include "roles.hpp"

std::shared_ptr<Promise<Role>> getUserServerRole(std::shared_ptr<IAgentRuntime> runtime, string entityId, string serverId)
{
    return coreGetUserServerRole(as<any>(runtime), entityId, serverId);
};


std::shared_ptr<Promise<any>> findWorldsForOwner(std::shared_ptr<IAgentRuntime> runtime, string entityId)
{
    return coreFindWorldsForOwner(as<any>(runtime), entityId);
};


