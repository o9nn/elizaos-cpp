#include "index.hpp"
#include <string>

std::shared_ptr<express::Router> agentsRouter(std::shared_ptr<Map<std::shared_ptr<UUID>, std::shared_ptr<IAgentRuntime>>> agents, std::shared_ptr<AgentServer> serverInstance)
{
    auto router = express->Router();
    router->use(std::string("/"), createAgentCrudRouter(agents, serverInstance));
    router->use(std::string("/"), createAgentLifecycleRouter(agents, serverInstance));
    router->use(std::string("/"), createAgentWorldsRouter(agents));
    router->use(std::string("/"), createAgentPanelsRouter(agents));
    router->use(std::string("/"), createAgentLogsRouter(agents));
    router->use(std::string("/"), createAgentCapabilitiesRouter(agents, serverInstance));
    router->use(std::string("/"), createAgentGoalsRouter(agents, serverInstance));
    router->use(std::string("/"), createAgentTodosRouter(agents, serverInstance));
    router->use(std::string("/"), createAgentSettingsRouter(agents, serverInstance));
    router->use(std::string("/"), createAgentKnowledgeRouter(agents, serverInstance));
    router->use(std::string("/"), createAgentPluginsRouter(agents, serverInstance));
    router->use(std::string("/"), createAgentMemoryRouter(agents));
    router->use(std::string("/"), createRoomManagementRouter(agents));
    router->use(std::string("/"), createAgentPortabilityRouter(agents, serverInstance));
    return router;
};


