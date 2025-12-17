#include "index.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

express::Router agentsRouter(Map<UUID agents, auto IAgentRuntime>, AgentServer serverInstance) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto router = express.Router();

    // Mount CRUD operations at root level
    router.use('/', createAgentCrudRouter(agents, serverInstance));

    // Mount lifecycle operations
    router.use('/', createAgentLifecycleRouter(agents, serverInstance));

    // Mount world management operations
    router.use('/', createAgentWorldsRouter(agents));

    // Mount panels operations
    router.use('/', createAgentPanelsRouter(agents));

    // Mount logs operations
    router.use('/', createAgentLogsRouter(agents));

    // Mount capabilities management
    router.use('/', createAgentCapabilitiesRouter(agents, serverInstance));

    // Mount goals management
    router.use('/', createAgentGoalsRouter(agents, serverInstance));

    // Mount todos management
    router.use('/', createAgentTodosRouter(agents, serverInstance));

    // Mount settings management
    router.use('/', createAgentSettingsRouter(agents, serverInstance));

    // Mount knowledge management
    router.use('/', createAgentKnowledgeRouter(agents, serverInstance));

    // Mount plugin management
    router.use('/', createAgentPluginsRouter(agents, serverInstance));

    // Mount memory operations
    router.use('/', createAgentMemoryRouter(agents));
    // Mount room management (list rooms and room details) under agents
    router.use('/', createRoomManagementRouter(agents));

    // Mount portability operations (export/import)
    router.use('/', createAgentPortabilityRouter(agents, serverInstance));

    return router;

}

} // namespace elizaos
