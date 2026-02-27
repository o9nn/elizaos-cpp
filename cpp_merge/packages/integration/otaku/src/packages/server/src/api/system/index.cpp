#include "/home/runner/work/elizaos-cpp/elizaos-cpp/otaku/src/packages/server/src/api/system/index.h"

std::shared_ptr<express::Router> systemRouter()
{
    auto router = express->Router();
    router->use(std::string("/env"), createEnvironmentRouter());
    router->use(std::string("/version"), createVersionRouter());
    return router;
};


