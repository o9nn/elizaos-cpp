#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/server/src/api/memory/index.h"

std::shared_ptr<express::Router> memoryRouter(std::shared_ptr<Map<std::shared_ptr<UUID>, std::shared_ptr<IAgentRuntime>>> agents, std::shared_ptr<AgentServer> serverInstance)
{
    auto router = express->Router();
    router->use(std:("/"), createAgentMemoryRouter(agents));
    router->use(std:("/"), createGroupMemoryRouter(agents, serverInstance));
    router->use(std:("/"), createRoomManagementRouter(agents));
    return router;
};


