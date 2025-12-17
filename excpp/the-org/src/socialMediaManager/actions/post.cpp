#include "post.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<> validateTwitterConfig(IAgentRuntime runtime, const std::string& serverId) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    isValid: boolean; error?: string
}

std::future<void> ensureTwitterClient(IAgentRuntime runtime, const std::string& serverId, const std::any& worldSettings) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto manager = runtime.getService("twitter");
        if (!manager) {
            throw std::runtime_error('Twitter client manager not found');
        }

        auto client = manager.getClient(serverId, runtime.agentId);

        if (!client) {
            std::cout << "Creating new Twitter client for server" << serverId << std::endl;
            client = manager.createClient(runtime, serverId, worldSettings);
            if (!client) {
                throw std::runtime_error('Failed to create Twitter client');
            }
        }

        return client;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
