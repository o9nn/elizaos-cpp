#include "index.hpp"

std::shared_ptr<express::Router> systemRouter()
{
    auto router = express->Router();
    router->use(std:("/env"), createEnvironmentRouter());
    router->use(std:("/version"), createVersionRouter());
    return router;
};


