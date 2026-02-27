#include "index.hpp"

std::shared_ptr<express::Router> audioRouter(std::shared_ptr<Map<std::shared_ptr<UUID>, std::shared_ptr<IAgentRuntime>>> agents)
{
    auto router = express->Router();
    router->use(std:("/"), createAudioProcessingRouter(agents));
    router->use(std:("/"), createSynthesisRouter(agents));
    router->use(std:("/"), createConversationRouter(agents));
    return router;
};


