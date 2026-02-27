#include "index.hpp"

std::shared_ptr<express::Router> agentsRouter(std::shared_ptr<Map<std::shared_ptr<UUID>, std::shared_ptr<IAgentRuntime>>> agents, std::shared_ptr<AgentServer> serverInstance)
{
    auto router = express->Router();
    router->use(std:("/"), createAgentCrudRouter(agents, serverInstance));
    router->use(std:("/"), createAgentLifecycleRouter(agents, serverInstance));
    router->use(std:("/"), createAgentWorldsRouter(agents));
    router->use(std:("/"), createAgentPanelsRouter(agents));
    router->use(std:("/"), createAgentLogsRouter(agents));
    router->use(std:("/"), createAgentMemoryRouter(agents));
    router->use(std:("/"), createRoomManagementRouter(agents));
    return router;
};


