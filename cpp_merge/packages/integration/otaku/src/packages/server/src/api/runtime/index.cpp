#include "index.hpp"

std::shared_ptr<express::Router> runtimeRouter(std::shared_ptr<ElizaOS> elizaOS, std::shared_ptr<AgentServer> serverInstance)
{
    auto router = express->Router();
    router->use(std:("/"), createHealthRouter(elizaOS, serverInstance));
    router->use(std:("/"), createLoggingRouter());
    router->use(std:("/debug"), createDebugRouter(serverInstance));
    return router;
};


