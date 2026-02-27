#include "index.hpp"
#include <string>

std::shared_ptr<express::Router> runtimeRouter(std::shared_ptr<Map<std::shared_ptr<UUID>, std::shared_ptr<IAgentRuntime>>> agents, std::shared_ptr<AgentServer> serverInstance)
{
    auto router = express->Router();
    router->use(std::string("/"), createHealthRouter(agents, serverInstance));
    router->use(std::string("/"), createLoggingRouter(agents, serverInstance));
    router->use(std::string("/debug"), createDebugRouter(serverInstance));
    return router;
};


