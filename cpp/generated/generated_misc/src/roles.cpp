#include "/home/runner/work/elizaos-cpp/elizaos-cpp/plugin-specification/core-plugin-v2/src/roles.h"

std::shared_ptr<Promise<Role>> getUserServerRole(std::shared_ptr<IAgentRuntime> runtime, std::string entityId, std::string serverId)
{
    return coreGetUserServerRole(as<any>(runtime), entityId, serverId);
};


std::shared_ptr<Promise<any>> findWorldsForOwner(std::shared_ptr<IAgentRuntime> runtime, std::string entityId)
{
    return coreFindWorldsForOwner(as<any>(runtime), entityId);
};


