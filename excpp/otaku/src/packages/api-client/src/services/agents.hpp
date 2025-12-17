#pragma once
#include <algorithm>
#include <any>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "elizaos/core.hpp"
#include "lib/base-client.hpp"
#include "types/agents.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



class AgentsService {
public:
    Promise< listAgents();
    std::future<Agent> getAgent(UUID agentId);
    std::future<Agent> createAgent(AgentCreateParams params);
    std::future<Agent> updateAgent(UUID agentId, AgentUpdateParams params);
    Promise< deleteAgent(UUID agentId);
    Promise< startAgent(UUID agentId);
    Promise< stopAgent(UUID agentId);
    Promise< getWorlds();
    Promise< addAgentToWorld(UUID agentId, UUID worldId);
    std::future<AgentWorldSettings> updateAgentWorldSettings(UUID agentId, UUID worldId, const std::unordered_map<std::string, std::any>& settings);
    Promise< getAgentPanels(UUID agentId);
    std::future<std::vector<AgentLog>> getAgentLogs(UUID agentId, std::optional<AgentLogsParams> params);
    Promise< deleteAgentLog(UUID agentId, UUID logId);
    Promise< getAgentsForServer(UUID serverId);
    Promise< addAgentToServer(UUID serverId, UUID agentId);
    Promise< removeAgentFromServer(UUID serverId, UUID agentId);
    Promise< getServersForAgent(UUID agentId);

private:
    bool success_;
    bool success_;
};


} // namespace elizaos
