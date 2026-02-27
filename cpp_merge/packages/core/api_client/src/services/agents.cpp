#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/api-client/src/services/agents.h"

std::shared_ptr<Promise<object>> AgentsService::listAgents()
{
    return this->get<object>(std:("/api/agents"));
}

std::shared_ptr<Promise<std::shared_ptr<Agent>>> AgentsService::getAgent(std::shared_ptr<UUID> agentId)
{
    return this->get<std::shared_ptr<Agent>>(std:("/api/agents/") + agentId + string_empty);
}

std::shared_ptr<Promise<std::shared_ptr<Agent>>> AgentsService::createAgent(std::shared_ptr<AgentCreateParams> params)
{
    return this->post<std::shared_ptr<Agent>>(std:("/api/agents"), params);
}

std::shared_ptr<Promise<std::shared_ptr<Agent>>> AgentsService::updateAgent(std::shared_ptr<UUID> agentId, std::shared_ptr<AgentUpdateParams> params)
{
    return this->patch<std::shared_ptr<Agent>>(std:("/api/agents/") + agentId + string_empty, params);
}

std::shared_ptr<Promise<object>> AgentsService::deleteAgent(std::shared_ptr<UUID> agentId)
{
    return this->delete<object>(std:("/api/agents/") + agentId + string_empty);
}

std::shared_ptr<Promise<object>> AgentsService::startAgent(std::shared_ptr<UUID> agentId)
{
    return this->post<object>(std:("/api/agents/") + agentId + std:("/start"));
}

std::shared_ptr<Promise<object>> AgentsService::stopAgent(std::shared_ptr<UUID> agentId)
{
    return this->post<object>(std:("/api/agents/") + agentId + std:("/stop"));
}

std::shared_ptr<Promise<object>> AgentsService::getWorlds()
{
    return this->get<object>(std:("/api/agents/worlds"));
}

std::shared_ptr<Promise<object>> AgentsService::addAgentToWorld(std::shared_ptr<UUID> agentId, std::shared_ptr<UUID> worldId)
{
    return this->post<object>(std:("/api/agents/") + agentId + std:("/worlds"), object{
        object::pair{std:("worldId"), std:("worldId")}
    });
}

std::shared_ptr<Promise<std::shared_ptr<AgentWorldSettings>>> AgentsService::updateAgentWorldSettings(std::shared_ptr<UUID> agentId, std::shared_ptr<UUID> worldId, Record<string, any> settings)
{
    return this->patch<std::shared_ptr<AgentWorldSettings>>(std:("/api/agents/") + agentId + std:("/worlds/") + worldId + string_empty, object{
        object::pair{std:("settings"), std:("settings")}
    });
}

std::shared_ptr<Promise<object>> AgentsService::getAgentPanels(std::shared_ptr<UUID> agentId)
{
    auto response = std::async([=]() { this->get<array<object>>(std:("/api/agents/") + agentId + std:("/panels")); });
    auto panels = ((Array->isArray(response)) ? response : array<any>())->map([=](auto panel, auto index) mutable
    {
        return (object{
            object::pair{std:("id"), string_empty + panel["name"] + std:("-") + index + string_empty}, 
            object::pair{std:("name"), panel["name"]}, 
            object::pair{std:("url"), panel["path"]}, 
            object::pair{std:("type"), std:("plugin")}
        });
    }
    );
    return object{
        object::pair{std:("panels"), std:("panels")}
    };
}

std::shared_ptr<Promise<array<std::shared_ptr<AgentLog>>>> AgentsService::getAgentLogs(std::shared_ptr<UUID> agentId, std::shared_ptr<AgentLogsParams> params)
{
    return this->get<array<std::shared_ptr<AgentLog>>>(std:("/api/agents/") + agentId + std:("/logs"), object{
        object::pair{std:("params"), std:("params")}
    });
}

std::shared_ptr<Promise<object>> AgentsService::deleteAgentLog(std::shared_ptr<UUID> agentId, std::shared_ptr<UUID> logId)
{
    return this->delete<object>(std:("/api/agents/") + agentId + std:("/logs/") + logId + string_empty);
}

std::shared_ptr<Promise<object>> AgentsService::getAgentsForServer(std::shared_ptr<UUID> serverId)
{
    return this->get<object>(std:("/api/messaging/servers/") + serverId + std:("/agents"));
}

std::shared_ptr<Promise<object>> AgentsService::addAgentToServer(std::shared_ptr<UUID> serverId, std::shared_ptr<UUID> agentId)
{
    return this->post<object>(std:("/api/messaging/servers/") + serverId + std:("/agents"), object{
        object::pair{std:("agentId"), std:("agentId")}
    });
}

std::shared_ptr<Promise<object>> AgentsService::removeAgentFromServer(std::shared_ptr<UUID> serverId, std::shared_ptr<UUID> agentId)
{
    return this->delete<object>(std:("/api/messaging/servers/") + serverId + std:("/agents/") + agentId + string_empty);
}

std::shared_ptr<Promise<object>> AgentsService::getServersForAgent(std::shared_ptr<UUID> agentId)
{
    return this->get<object>(std:("/api/messaging/agents/") + agentId + std:("/servers"));
}

AgentsService::AgentsService(std::shared_ptr<ApiClientConfig> config) : BaseApiClient(config) {
}

