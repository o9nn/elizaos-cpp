#include "roles.hpp"
#include <vector>
#include <future>
#include <optional>
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<Role> getUserServerRole(IAgentRuntime runtime, const std:& entityId, const std:& serverId) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return coreGetUserServerRole(runtime, entityId, serverId);

}

std::future<std::optional<std::vector<World>>> findWorldsForOwner(IAgentRuntime runtime, const std:& entityId) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return coreFindWorldsForOwner(runtime, entityId);

}

} // namespace elizaos
