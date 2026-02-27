#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/agentserver/src/api/audio/index.h"

std::shared_ptr<express::Router> audioRouter(std::shared_ptr<Map<std::shared_ptr<UUID>, std::shared_ptr<IAgentRuntime>>> agents)
{
    auto router = express->Router();
    router->use(std:("/"), createAudioProcessingRouter(agents));
    router->use(std:("/"), createSynthesisRouter(agents));
    router->use(std:("/"), createConversationRouter(agents));
    return router;
};


