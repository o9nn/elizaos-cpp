#include "updateFormat.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

UUID getStorageRoomId(IAgentRuntime runtime, const std::string& serverId) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Create a consistent hash based on serverId
    const auto serverHash = serverId.replace(/[^a-zA-Z0-9]/g, "");
    return "createUniqueUuid(runtime, " + "store-team-members-" + serverHash;

}

} // namespace elizaos
