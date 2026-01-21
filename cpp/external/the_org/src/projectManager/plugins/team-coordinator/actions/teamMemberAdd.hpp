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
    std::string createdAt;
    std::optional<std::vector<std::string>> updatesFormat;
};

struct TeamMemberConfig {
    std::vector<TeamMember> teamMembers;
    double lastUpdated;
    std::string serverId;
};

/**
 * Creates a consistent room ID for team members storage
 * @param serverId The server ID
 * @returns A consistent room ID string
 */
UUID getTeamMembersRoomId(IAgentRuntime runtime, const std::string& serverId);

/**
 * Fetches team members for a specific server from memory
 * @param runtime The agent runtime
 * @param serverId The ID of the server to fetch team members for
 * @returns An array of team members
 */

/**
 * Checks if a team member already exists in the database
 * @param existingMembers The list of existing team members
 * @param newMember The new team member to check
 * @returns True if the member already exists, false otherwise
 */
bool isDuplicateTeamMember(const std::vector<TeamMember>& existingMembers, TeamMember newMember);

      // Basic validation

      // Store server ID in state for the handler

      // Check if user is an admin

      // Extract user message

      // Get server ID from state

      // Example parsing team member data from input
        
          // questions format can be in this form Main Priority for next week , What did you get done this week? or Blockers

        // TODO : after this all things are to be done like recordChecKInAction fuck this code

        // Parse the response

          // Fix: Ensure teamMembers is an array

        // Validate that either TG or Discord name is present for each team member
          // Fix: Check for required fields

          // if (!member.format) {
          //   logger.warn(`Skipping team member in section "${member.section}" missing format`);
          //   return false;
          // }

        // Add table name to getMemories call

            // Store all the validatedTeamMembers in the config

            // First create the room to avoid foreign key constraint error

          // Fetch existing team members from the config

          // Filter out team members that already exist

            // Format team members into a readable list

            // Add a callback here to respond when no new members are added
            // Add new team members to the existing list

            // Update the config with the new team members

            // Update the memory with the new config

            // Format the newly added team members for the response

            // Add a callback here to respond when new members are added

          // Return early since we've already sent the callback

        // Fetch the updated team members to include in the response

          // Format all team members for the response


} // namespace elizaos
