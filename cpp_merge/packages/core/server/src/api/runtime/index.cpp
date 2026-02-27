#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/server/src/api/runtime/index.h"

std::shared_ptr<express::Router> runtimeRouter(std::shared_ptr<Map<std::shared_ptr<UUID>, std::shared_ptr<IAgentRuntime>>> agents, std::shared_ptr<AgentServer> serverInstance)
{
    auto router = express->Router();
    router->use(std:("/"), createHealthRouter(agents, serverInstance));
    router->use(std:("/"), createLoggingRouter(agents, serverInstance));
    router->use(std:("/debug"), createDebugRouter(serverInstance));
    return router;
};


