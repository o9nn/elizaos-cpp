#include "index.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

express::Router memoryRouter(ElizaOS elizaOS, AgentServer serverInstance) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto router = express.Router();

    // Mount agent memory management at root level
    router.use("/", createAgentMemoryRouter(elizaOS, serverInstance));

    // Mount group memory management
    router.use("/", createGroupMemoryRouter(elizaOS, serverInstance));

    // Mount room management
    router.use("/", createRoomManagementRouter(elizaOS));

    return router;

}

} // namespace elizaos
