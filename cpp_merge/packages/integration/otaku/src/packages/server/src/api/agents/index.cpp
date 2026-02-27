#include "/home/runner/work/elizaos-cpp/elizaos-cpp/otaku/src/packages/server/src/api/agents/index.h"

std::shared_ptr<express::Router> agentsRouter(std::shared_ptr<ElizaOS> elizaOS, std::shared_ptr<AgentServer> serverInstance)
{
    auto router = express->Router();
    router->use(std:("/"), createAgentCrudRouter(elizaOS, serverInstance));
    router->use(std:("/"), createAgentLifecycleRouter(elizaOS, serverInstance));
    router->use(std:("/"), createAgentWorldsRouter(elizaOS));
    router->use(std:("/"), createAgentPanelsRouter(elizaOS));
    router->use(std:("/"), createAgentLogsRouter(elizaOS));
    router->use(std:("/"), createAgentRunsRouter(elizaOS));
    router->use(std:("/"), createAgentMemoryRouter(elizaOS));
    router->use(std:("/"), createRoomManagementRouter(elizaOS));
    return router;
};


