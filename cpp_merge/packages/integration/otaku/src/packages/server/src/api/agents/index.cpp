#include "index.hpp"
#include <string>

std::shared_ptr<express::Router> agentsRouter(std::shared_ptr<ElizaOS> elizaOS, std::shared_ptr<AgentServer> serverInstance)
{
    auto router = express->Router();
    router->use(std::string("/"), createAgentCrudRouter(elizaOS, serverInstance));
    router->use(std::string("/"), createAgentLifecycleRouter(elizaOS, serverInstance));
    router->use(std::string("/"), createAgentWorldsRouter(elizaOS));
    router->use(std::string("/"), createAgentPanelsRouter(elizaOS));
    router->use(std::string("/"), createAgentLogsRouter(elizaOS));
    router->use(std::string("/"), createAgentRunsRouter(elizaOS));
    router->use(std::string("/"), createAgentMemoryRouter(elizaOS));
    router->use(std::string("/"), createRoomManagementRouter(elizaOS));
    return router;
};


