#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_API-CLIENT_SRC_SERVICES_AGENTS_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_API-CLIENT_SRC_SERVICES_AGENTS_H
#include "core.h"
#include "@elizaos/core.h"
#include "../lib/base-client.h"
#include "../types/agents.h"

class AgentsService;

class AgentsService : public BaseApiClient, public std::enable_shared_from_this<AgentsService> {
public:
    using std::enable_shared_from_this<AgentsService>::shared_from_this;
    virtual std::shared_ptr<Promise<object>> listAgents();
    virtual std::shared_ptr<Promise<std::shared_ptr<Agent>>> getAgent(std::shared_ptr<UUID> agentId);
    virtual std::shared_ptr<Promise<std::shared_ptr<Agent>>> createAgent(std::shared_ptr<AgentCreateParams> params);
    virtual std::shared_ptr<Promise<std::shared_ptr<Agent>>> updateAgent(std::shared_ptr<UUID> agentId, std::shared_ptr<AgentUpdateParams> params);
    virtual std::shared_ptr<Promise<object>> deleteAgent(std::shared_ptr<UUID> agentId);
    virtual std::shared_ptr<Promise<object>> startAgent(std::shared_ptr<UUID> agentId);
    virtual std::shared_ptr<Promise<object>> stopAgent(std::shared_ptr<UUID> agentId);
    virtual std::shared_ptr<Promise<object>> getWorlds();
    virtual std::shared_ptr<Promise<object>> addAgentToWorld(std::shared_ptr<UUID> agentId, std::shared_ptr<UUID> worldId);
    virtual std::shared_ptr<Promise<std::shared_ptr<AgentWorldSettings>>> updateAgentWorldSettings(std::shared_ptr<UUID> agentId, std::shared_ptr<UUID> worldId, Record<string, any> settings);
    virtual std::shared_ptr<Promise<object>> getAgentPanels(std::shared_ptr<UUID> agentId);
    virtual std::shared_ptr<Promise<array<std::shared_ptr<AgentLog>>>> getAgentLogs(std::shared_ptr<UUID> agentId, std::shared_ptr<AgentLogsParams> params = undefined);
    virtual std::shared_ptr<Promise<object>> deleteAgentLog(std::shared_ptr<UUID> agentId, std::shared_ptr<UUID> logId);
    virtual std::shared_ptr<Promise<object>> getAgentsForServer(std::shared_ptr<UUID> serverId);
    virtual std::shared_ptr<Promise<object>> addAgentToServer(std::shared_ptr<UUID> serverId, std::shared_ptr<UUID> agentId);
    virtual std::shared_ptr<Promise<object>> removeAgentFromServer(std::shared_ptr<UUID> serverId, std::shared_ptr<UUID> agentId);
    virtual std::shared_ptr<Promise<object>> getServersForAgent(std::shared_ptr<UUID> agentId);
    AgentsService(std::shared_ptr<ApiClientConfig> config);
};

#endif
