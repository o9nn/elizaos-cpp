#include "index.hpp"
#include <string>

std::shared_ptr<express::Router> mediaRouter()
{
    auto router = express->Router();
    router->use(std::string("/agents"), createAgentMediaRouter());
    router->use(std::string("/channels"), createChannelMediaRouter());
    return router;
};


