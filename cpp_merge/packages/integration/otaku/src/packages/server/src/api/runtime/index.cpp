#include "index.hpp"
#include <string>

std::shared_ptr<express::Router> runtimeRouter(std::shared_ptr<ElizaOS> elizaOS, std::shared_ptr<AgentServer> serverInstance)
{
    auto router = express->Router();
    router->use(std::string("/"), createHealthRouter(elizaOS, serverInstance));
    router->use(std::string("/"), createLoggingRouter());
    router->use(std::string("/debug"), createDebugRouter(serverInstance));
    return router;
};


