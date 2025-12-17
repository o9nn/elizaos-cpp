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

      // Basic validation

      // Store server ID in state for the handler

      // Get server ID from state

      // FIXED: Get the correct room ID for storing team members

      // Get memories from the team members storage room

      // Find the team members config memory

      // Extract and format team members

      // Group team members by section

      // Format the response

      // Flat list of team members in requested format

      // Send the response


} // namespace elizaos
