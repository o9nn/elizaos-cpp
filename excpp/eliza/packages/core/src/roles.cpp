#include "roles.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<Role> getUserServerRole(IAgentRuntime runtime, const std::string& entityId, const std::string& serverId) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto worldId = createUniqueUuid(runtime, serverId);
    const auto world = runtime.getWorld(worldId);

    if (!world || !world.metadata.roles) {
        return Role.NONE;
    }

    if (world.metadata.roles[entityId]) {
        return world.metadata.roles[entityId];
    }

    // Also check original ID format
    if (world.metadata.roles[entityId]) {
        return world.metadata.roles[entityId];
    }

    return Role.NONE;

}

std::future<std::optional<std::vector<World>>> findWorldsForOwner(IAgentRuntime runtime, const std::string& entityId) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!entityId) {
        std::cerr << 'User ID is required to find server' << std::endl;
        return nullptr;
    }

    // Get all worlds for this agent
    const auto worlds = runtime.getAllWorlds();

    if (!worlds || worlds.length == 0) {
        std::cout << 'No worlds found for this agent' << std::endl;
        return nullptr;
    }

    const auto ownerWorlds = [];
    // Find world where the user is the owner
    for (const auto& world : worlds)
        if (world.metadata.ownership.ownerId == entityId) {
            ownerWorlds.push(world);
        }
    }

    return ownerWorlds.length ? ownerWorlds : nullptr;

}

} // namespace elizaos
