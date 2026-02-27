#include "/home/runner/work/elizaos-cpp/elizaos-cpp/otaku/src/packages/server/src/api/audio/index.h"

std::shared_ptr<express::Router> audioRouter(std::shared_ptr<ElizaOS> elizaOS)
{
    auto router = express->Router();
    router->use(std::string("/"), createAudioProcessingRouter(elizaOS));
    router->use(std::string("/"), createSynthesisRouter(elizaOS));
    router->use(std::string("/"), createConversationRouter(elizaOS));
    return router;
};


