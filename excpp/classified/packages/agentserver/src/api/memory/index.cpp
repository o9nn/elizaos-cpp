#include "index.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

express::Router memoryRouter(const std::unordered_map<UUID, IAgentRuntime>& agents, AgentServer serverInstance) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto router = express.Router();

    // Mount agent memory management at root level
    router.use('/', createAgentMemoryRouter(agents));

    // Mount group memory management
    router.use('/', createGroupMemoryRouter(agents, serverInstance));

    // Mount room management
    router.use('/', createRoomManagementRouter(agents));

    return router;

}

} // namespace elizaos
