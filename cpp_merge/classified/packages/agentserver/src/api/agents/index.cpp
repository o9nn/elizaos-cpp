#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/agentserver/src/api/agents/index.h"

std::shared_ptr<express::Router> agentsRouter(std::shared_ptr<Map<std::shared_ptr<UUID>, std::shared_ptr<IAgentRuntime>>> agents, std::shared_ptr<AgentServer> serverInstance)
{
    auto router = express->Router();
    router->use(std:("/"), createAgentCrudRouter(agents, serverInstance));
    router->use(std:("/"), createAgentLifecycleRouter(agents, serverInstance));
    router->use(std:("/"), createAgentWorldsRouter(agents));
    router->use(std:("/"), createAgentPanelsRouter(agents));
    router->use(std:("/"), createAgentLogsRouter(agents));
    router->use(std:("/"), createAgentCapabilitiesRouter(agents, serverInstance));
    router->use(std:("/"), createAgentGoalsRouter(agents, serverInstance));
    router->use(std:("/"), createAgentTodosRouter(agents, serverInstance));
    router->use(std:("/"), createAgentSettingsRouter(agents, serverInstance));
    router->use(std:("/"), createAgentKnowledgeRouter(agents, serverInstance));
    router->use(std:("/"), createAgentPluginsRouter(agents, serverInstance));
    router->use(std:("/"), createAgentMemoryRouter(agents));
    router->use(std:("/"), createRoomManagementRouter(agents));
    router->use(std:("/"), createAgentPortabilityRouter(agents, serverInstance));
    return router;
};


