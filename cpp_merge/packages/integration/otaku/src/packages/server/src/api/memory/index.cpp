#include "index.hpp"

std::shared_ptr<express::Router> memoryRouter(std::shared_ptr<ElizaOS> elizaOS, std::shared_ptr<AgentServer> serverInstance)
{
    auto router = express->Router();
    router->use(std:("/"), createAgentMemoryRouter(elizaOS, serverInstance));
    router->use(std:("/"), createGroupMemoryRouter(elizaOS, serverInstance));
    router->use(std:("/"), createRoomManagementRouter(elizaOS));
    return router;
};


