#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/agentserver/src/api/media/index.h"

std::shared_ptr<express::Router> mediaRouter()
{
    auto router = express->Router();
    router->use(std:("/agents"), createAgentMediaRouter());
    router->use(std:("/channels"), createChannelMediaRouter());
    return router;
};


