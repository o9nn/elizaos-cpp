#include "validation.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<std::vector<AgentBasic>> getAgents(OptionValues opts) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto baseUrl = getAgentsBaseUrl(opts);
        const auto response = fetch(baseUrl);
        if (!response.ok) {
            throw std::runtime_error(`Failed to fetch agents list: ${response.statusText}`);
        }
        const auto rawData = response.json();
        const auto validatedData = AgentsListResponseSchema.parse(rawData);
        return (validatedData.data.agents || [])[];

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<std::string> resolveAgentId(const std::string& idOrNameOrIndex, OptionValues opts) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        // First try to get all agents to find by name
        const auto agents = getAgents(opts);

        // Try to find agent by name
        const auto agentByName = agents.find(;
        [&](agent) { return agent.name.toLowerCase() == idOrNameOrIndex.toLowerCase(); }
        );

        if (agentByName) {
            return agentByName.id;
        }

        // Try to find agent by ID
        const auto agentById = agents.find((agent) => agent.id == idOrNameOrIndex);

        if (agentById) {
            return agentById.id;
        }

        // Try to find agent by index
        if (!Number.isNaN(Number(idOrNameOrIndex))) {
            const auto indexAgent = agents[Number(idOrNameOrIndex)];
            if (indexAgent) {
                return indexAgent.id;
            }
        }

        // If no agent is found, throw a specific error type that we can catch
        throw std::runtime_error(`AGENT_NOT_FOUND:${idOrNameOrIndex}`);

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
