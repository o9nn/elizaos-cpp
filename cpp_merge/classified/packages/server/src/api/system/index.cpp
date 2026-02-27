#include "index.hpp"
#include <string>

std::shared_ptr<express::Router> systemRouter()
{
    auto router = express->Router();
    router->use(std::string("/env"), createEnvironmentRouter());
    router->use(std::string("/version"), createVersionRouter());
    return router;
};


