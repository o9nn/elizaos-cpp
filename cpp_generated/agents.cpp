#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/api-client/src/services/agents.h"

std::shared_ptr<Promise<object>> AgentsService::listAgents()
{
    return this->get<object>(std::string("/api/agents"));
}

std::shared_ptr<Promise<std::shared_ptr<Agent>>> AgentsService::getAgent(std::shared_ptr<UUID> agentId)
{
    return this->get<std::shared_ptr<Agent>>(std::string("/api/agents/") + agentId + string_empty);
}

std::shared_ptr<Promise<std::shared_ptr<Agent>>> AgentsService::createAgent(std::shared_ptr<AgentCreateParams> params)
{
    return this->post<std::shared_ptr<Agent>>(std::string("/api/agents"), params);
}

std::shared_ptr<Promise<std::shared_ptr<Agent>>> AgentsService::updateAgent(std::shared_ptr<UUID> agentId, std::shared_ptr<AgentUpdateParams> params)
{
    return this->patch<std::shared_ptr<Agent>>(std::string("/api/agents/") + agentId + string_empty, params);
}

std::shared_ptr<Promise<object>> AgentsService::deleteAgent(std::shared_ptr<UUID> agentId)
{
    return this->delete<object>(std::string("/api/agents/") + agentId + string_empty);
}

std::shared_ptr<Promise<object>> AgentsService::startAgent(std::shared_ptr<UUID> agentId)
{
    return this->post<object>(std::string("/api/agents/") + agentId + std::string("/start"));
}

std::shared_ptr<Promise<object>> AgentsService::stopAgent(std::shared_ptr<UUID> agentId)
{
    return this->post<object>(std::string("/api/agents/") + agentId + std::string("/stop"));
}

std::shared_ptr<Promise<object>> AgentsService::getWorlds()
{
    return this->get<object>(std::string("/api/agents/worlds"));
}

std::shared_ptr<Promise<object>> AgentsService::addAgentToWorld(std::shared_ptr<UUID> agentId, std::shared_ptr<UUID> worldId)
{
    return this->post<object>(std::string("/api/agents/") + agentId + std::string("/worlds"), object{
        object::pair{std::string("worldId"), std::string("worldId")}
    });
}

std::shared_ptr<Promise<std::shared_ptr<AgentWorldSettings>>> AgentsService::updateAgentWorldSettings(std::shared_ptr<UUID> agentId, std::shared_ptr<UUID> worldId, Record<string, any> settings)
{
    return this->patch<std::shared_ptr<AgentWorldSettings>>(std::string("/api/agents/") + agentId + std::string("/worlds/") + worldId + string_empty, object{
        object::pair{std::string("settings"), std::string("settings")}
    });
}

std::shared_ptr<Promise<object>> AgentsService::getAgentPanels(std::shared_ptr<UUID> agentId)
{
    auto response = std::async([=]() { this->get<array<object>>(std::string("/api/agents/") + agentId + std::string("/panels")); });
    auto panels = ((Array->isArray(response)) ? response : array<any>())->map([=](auto panel, auto index) mutable
    {
        return (object{
            object::pair{std::string("id"), string_empty + panel["name"] + std::string("-") + index + string_empty}, 
            object::pair{std::string("name"), panel["name"]}, 
            object::pair{std::string("url"), panel["path"]}, 
            object::pair{std::string("type"), std::string("plugin")}
        });
    }
    );
    return object{
        object::pair{std::string("panels"), std::string("panels")}
    };
}

std::shared_ptr<Promise<array<std::shared_ptr<AgentLog>>>> AgentsService::getAgentLogs(std::shared_ptr<UUID> agentId, std::shared_ptr<AgentLogsParams> params)
{
    return this->get<array<std::shared_ptr<AgentLog>>>(std::string("/api/agents/") + agentId + std::string("/logs"), object{
        object::pair{std::string("params"), std::string("params")}
    });
}

std::shared_ptr<Promise<object>> AgentsService::deleteAgentLog(std::shared_ptr<UUID> agentId, std::shared_ptr<UUID> logId)
{
    return this->delete<object>(std::string("/api/agents/") + agentId + std::string("/logs/") + logId + string_empty);
}

std::shared_ptr<Promise<object>> AgentsService::getAgentsForServer(std::shared_ptr<UUID> serverId)
{
    return this->get<object>(std::string("/api/messaging/servers/") + serverId + std::string("/agents"));
}

std::shared_ptr<Promise<object>> AgentsService::addAgentToServer(std::shared_ptr<UUID> serverId, std::shared_ptr<UUID> agentId)
{
    return this->post<object>(std::string("/api/messaging/servers/") + serverId + std::string("/agents"), object{
        object::pair{std::string("agentId"), std::string("agentId")}
    });
}

std::shared_ptr<Promise<object>> AgentsService::removeAgentFromServer(std::shared_ptr<UUID> serverId, std::shared_ptr<UUID> agentId)
{
    return this->delete<object>(std::string("/api/messaging/servers/") + serverId + std::string("/agents/") + agentId + string_empty);
}

std::shared_ptr<Promise<object>> AgentsService::getServersForAgent(std::shared_ptr<UUID> agentId)
{
    return this->get<object>(std::string("/api/messaging/agents/") + agentId + std::string("/servers"));
}

AgentsService::AgentsService(std::shared_ptr<ApiClientConfig> config) : BaseApiClient(config) {
}

