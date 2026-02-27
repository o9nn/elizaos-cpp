#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/agentserver/src/api/memory/index.h"

std::shared_ptr<express::Router> memoryRouter(std::shared_ptr<Map<std::shared_ptr<UUID>, std::shared_ptr<IAgentRuntime>>> agents, std::shared_ptr<AgentServer> serverInstance)
{
    auto router = express->Router();
    router->use(std::string("/"), createAgentMemoryRouter(agents));
    router->use(std::string("/"), createGroupMemoryRouter(agents, serverInstance));
    router->use(std::string("/"), createRoomManagementRouter(agents));
    return router;
};


