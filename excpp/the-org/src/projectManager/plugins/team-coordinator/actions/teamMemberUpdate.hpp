#include "elizaos/core.hpp"
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Define a simple interface for the Discord service

struct ReportChannelConfig {
    std::optional<std::string> serverId;
    std::optional<std::string> channelId;
};

/**
 * Posts team member update to the configured Discord channel for the server
 */
std::future<bool> postUpdateToDiscordChannel(IAgentRuntime runtime, TeamMemberUpdate update);

std::future<bool> storeTeamMemberUpdate(IAgentRuntime runtime, TeamMemberUpdate update);

std::future<TeamMemberUpdate | null> parseTeamMemberUpdate(IAgentRuntime runtime, Memory message);

      // Parse the update from the message

        // Store the update in memory

        // Post update to configured Discord channel

        // Send confirmation

        await callback(content, []);

        // Handle other errors


} // namespace elizaos
