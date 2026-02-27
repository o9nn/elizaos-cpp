#include "index.hpp"

std::shared_ptr<express::Router> messagingRouter(std::shared_ptr<ElizaOS> elizaOS, std::shared_ptr<AgentServer> serverInstance)
{
    auto router = express->Router();
    if (!serverInstance) {
        throw any(std::make_shared<Error>(std:("ServerInstance is required for messaging router")));
    }
    router->use(std:("/"), createMessagingCoreRouter(serverInstance));
    router->use(std:("/"), createServersRouter(serverInstance));
    router->use(std:("/"), createChannelsRouter(elizaOS, serverInstance));
    router->use(std:("/"), createSessionsRouter(elizaOS, serverInstance));
    router->use(std:("/"), createJobsRouter(elizaOS, serverInstance));
    return router;
};


