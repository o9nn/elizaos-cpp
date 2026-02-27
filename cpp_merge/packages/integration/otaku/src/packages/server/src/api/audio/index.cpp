#include "index.hpp"

std::shared_ptr<express::Router> audioRouter(std::shared_ptr<ElizaOS> elizaOS)
{
    auto router = express->Router();
    router->use(std:("/"), createAudioProcessingRouter(elizaOS));
    router->use(std:("/"), createSynthesisRouter(elizaOS));
    router->use(std:("/"), createConversationRouter(elizaOS));
    return router;
};


