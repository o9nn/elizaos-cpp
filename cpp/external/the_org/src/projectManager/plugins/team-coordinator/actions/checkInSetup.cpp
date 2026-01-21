#include "checkInSetup.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<> validateDiscordConfig(IAgentRuntime runtime, const std::string& serverId) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    isValid: boolean; error?: string
}

std::future<DiscordService> ensureDiscordClient(IAgentRuntime runtime) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        std::cout << "Ensuring Discord client is available" << std::endl;

        try {
            const auto discordService = runtime.getService("discord");
            std::cout << "Discord service found: " + std::to_string(!!discordService) << std::endl;

            if (!discordService) {
                std::cerr << "Discord service not found in runtime" << std::endl;
                throw std::runtime_error('Discord service not found');
            }

            // Log what's in the service to see its structure
            std::cout << "Discord service structure: " + std::to_string(/* JSON.stringify */ std::string(Object.keys(discordService))) << std::endl;

            // Check if client exists and is ready
            std::cout << "Discord client exists: " + std::to_string(!!discordService.client) << std::endl;
            if (!discordService.client) {
                std::cerr << "Discord client not initialized in service" << std::endl;
                throw std::runtime_error('Discord client not initialized');
            }

            std::cout << "Discord client successfully validated" << std::endl;
            return discordService;
            } catch (error: unknown) {
                const auto err = error;
                std::cerr << "Error ensuring Discord client: " + std::to_string(err.message || "Unknown error") << std::endl;
                std::cerr << "Error stack: " + std::to_string(err.stack || "No stack trace available") << std::endl;
                throw;
            }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
