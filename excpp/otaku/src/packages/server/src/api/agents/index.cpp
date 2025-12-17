#include "index.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

express::Router agentsRouter(ElizaOS elizaOS, AgentServer serverInstance) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto router = express.Router();

    // Mount CRUD operations at root level
    router.use("/", createAgentCrudRouter(elizaOS, serverInstance));

    // Mount lifecycle operations
    router.use("/", createAgentLifecycleRouter(elizaOS, serverInstance));

    // Mount world management operations
    router.use("/", createAgentWorldsRouter(elizaOS));

    // Mount panels operations
    router.use("/", createAgentPanelsRouter(elizaOS));

    // Mount logs operations
    router.use("/", createAgentLogsRouter(elizaOS));

    // Mount runs operations
    router.use("/", createAgentRunsRouter(elizaOS));

    // Mount memory operations
    router.use("/", createAgentMemoryRouter(elizaOS));
    // Mount room management (list rooms and room details) under agents
    router.use("/", createRoomManagementRouter(elizaOS));

    return router;

}

} // namespace elizaos
