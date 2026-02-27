#include "index.hpp"

std::shared_ptr<express::Router> mediaRouter()
{
    auto router = express->Router();
    router->use(std:("/agents"), createAgentMediaRouter());
    router->use(std:("/channels"), createChannelMediaRouter());
    return router;
};


