#include "/home/runner/work/elizaos-cpp/elizaos-cpp/otaku/src/packages/server/src/api/audio/index.h"

std::shared_ptr<express::Router> audioRouter(std::shared_ptr<ElizaOS> elizaOS)
{
    auto router = express->Router();
    router->use(std:("/"), createAudioProcessingRouter(elizaOS));
    router->use(std:("/"), createSynthesisRouter(elizaOS));
    router->use(std:("/"), createConversationRouter(elizaOS));
    return router;
};


