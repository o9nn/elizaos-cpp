#include "index.hpp"
#include <string>

std::shared_ptr<express::Router> audioRouter(std::shared_ptr<Map<std::shared_ptr<UUID>, std::shared_ptr<IAgentRuntime>>> agents)
{
    auto router = express->Router();
    router->use(std::string("/"), createAudioProcessingRouter(agents));
    router->use(std::string("/"), createSynthesisRouter(agents));
    router->use(std::string("/"), createConversationRouter(agents));
    return router;
};


