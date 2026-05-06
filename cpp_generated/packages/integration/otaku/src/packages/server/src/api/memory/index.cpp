#include "/home/runner/work/elizaos-cpp/elizaos-cpp/otaku/src/packages/server/src/api/memory/index.h"

std::shared_ptr<express::Router> memoryRouter(std::shared_ptr<ElizaOS> elizaOS, std::shared_ptr<AgentServer> serverInstance)
{
    auto router = express->Router();
    router->use(std::string("/"), createAgentMemoryRouter(elizaOS, serverInstance));
    router->use(std::string("/"), createGroupMemoryRouter(elizaOS, serverInstance));
    router->use(std::string("/"), createRoomManagementRouter(elizaOS));
    return router;
};


