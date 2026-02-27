#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/agentserver/src/api/messaging/index.h"

std::shared_ptr<express::Router> messagingRouter(std::shared_ptr<Map<std::shared_ptr<UUID>, std::shared_ptr<IAgentRuntime>>> agents, std::shared_ptr<AgentServer> serverInstance)
{
    auto router = express->Router();
    if (!serverInstance) {
        throw any(std::make_shared<Error>(std::string("ServerInstance is required for messaging router")));
    }
    router->use(std::string("/"), createMessagingCoreRouter(serverInstance));
    router->use(std::string("/"), createServersRouter(serverInstance));
    router->use(std::string("/"), createChannelsRouter(agents, serverInstance));
    return router;
};


