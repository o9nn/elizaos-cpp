#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/server/src/api/system/index.h"

std::shared_ptr<express::Router> systemRouter()
{
    auto router = express->Router();
    router->use(std:("/env"), createEnvironmentRouter());
    return router;
};


