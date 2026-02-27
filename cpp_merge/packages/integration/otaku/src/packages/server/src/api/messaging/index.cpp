#include "index.hpp"
#include <string>

std::shared_ptr<express::Router> messagingRouter(std::shared_ptr<ElizaOS> elizaOS, std::shared_ptr<AgentServer> serverInstance)
{
    auto router = express->Router();
    if (!serverInstance) {
        throw any(std::make_shared<Error>(std::string("ServerInstance is required for messaging router")));
    }
    router->use(std::string("/"), createMessagingCoreRouter(serverInstance));
    router->use(std::string("/"), createServersRouter(serverInstance));
    router->use(std::string("/"), createChannelsRouter(elizaOS, serverInstance));
    router->use(std::string("/"), createSessionsRouter(elizaOS, serverInstance));
    router->use(std::string("/"), createJobsRouter(elizaOS, serverInstance));
    return router;
};


