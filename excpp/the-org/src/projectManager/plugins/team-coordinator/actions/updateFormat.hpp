#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct TeamMember {
    std::string section;
    std::optional<std::string> tgName;
    std::optional<std::string> discordName;
    std::string format;
    std::string serverId;
    std::optional<std::string> serverName;
    std::optional<std::string> createdAt;
    std::optional<std::vector<std::string>> updatesFormat;
};

/**
 * Creates a consistent room ID for team members storage
 * @param runtime The agent runtime
 * @param serverId The server ID
 * @returns A consistent room ID string
 */
UUID getStorageRoomId(IAgentRuntime runtime, const std::string& serverId);

      // Store the user ID in state for the handler

      // Log the telegram and discord metadata for debugging

      // Get all memories using agentId since roomId is not available

      // Filter to get only team member configs

      // Initialize an empty array for all team members across all servers

      // Extract all team members from all configs

      // Log team members and username for debugging

      // Try to find the team member by username

      // Format the response


} // namespace elizaos
