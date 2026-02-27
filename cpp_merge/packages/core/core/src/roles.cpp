#include "roles.hpp"

std::shared_ptr<Promise<Role>> getUserServerRole(std::shared_ptr<IAgentRuntime> runtime, string entityId, string serverId)
{
    auto worldId = createUniqueUuid(runtime, serverId);
    auto world = std::async([=]() { runtime->getWorld(worldId); });
    if (OR((!world), (!world["metadata"]["roles"]))) {
        return Role::NONE;
    }
    if (const_(world["metadata"]["roles"])[entityId]) {
        return as<Role>(const_(world["metadata"]["roles"])[entityId]);
    }
    if (const_(world["metadata"]["roles"])[entityId]) {
        return as<Role>(const_(world["metadata"]["roles"])[entityId]);
    }
    return Role::NONE;
};


std::shared_ptr<Promise<any>> findWorldsForOwner(std::shared_ptr<IAgentRuntime> runtime, string entityId)
{
    if (!entityId) {
        logger->error(std:("User ID is required to find server"));
        return nullptr;
    }
    auto worlds = std::async([=]() { runtime->getAllWorlds(); });
    if (OR((!worlds), (worlds->get_length() == 0))) {
        logger->info(std:("No worlds found for this agent"));
        return nullptr;
    }
    auto ownerWorlds = array<any>();
    for (auto& world : worlds)
    {
        if (world["metadata"]["ownership"]["ownerId"] == entityId) {
            ownerWorlds->push(world);
        }
    }
    return (ownerWorlds->get_length()) ? any(ownerWorlds) (nullptr);
};


