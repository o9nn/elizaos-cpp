#pragma once
#include <algorithm>
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

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct DiscordComponentInteraction {
    std::string customId;
    double componentType;
    std::optional<std::vector<std::string>> values;
};

struct ReportChannelConfig {
    std::optional<std::string> serverId;
    std::optional<std::string> serverName;
    std::string channelId;
    std::string createdAt;
    std::optional<std::string> source;
};

struct CheckInSchedule {
    std::string type;
    UUID scheduleId;
    std::optional<std::string> teamMemberUserName;
    std::string checkInType;
    std::string channelId;
    std::string frequency;
    std::string checkInTime;
    std::string source;
    std::string createdAt;
    std::string serverId;
};

// Required Discord configuration fields

/**
 * Validates the Discord configuration for a specific server.
 * @param {IAgentRuntime} runtime - The Agent runtime.
 * @param {string} serverId - The ID of the server to validate.
 * @returns {Promise<{ isValid: boolean; error?: string }>}
 */
    // logger.info(`Validating Discord config for server ${serverId}`);

    // // Check required fields
    // for (const field of REQUIRED_DISCORD_FIELDS) {
    //       isValid: false,
    //       error: `Missing required Discord configuration: ${field}`,
    //     };
    //   }
    // }

/**
 * Ensures a Discord client exists and is ready
 * @param {IAgentRuntime} runtime - The Agent runtime
 * @returns {Promise<DiscordService>} The Discord client
 */
std::future<DiscordService> ensureDiscordClient(IAgentRuntime runtime);

      // Existing validation code...

      // Check if user is an admin

      // // Define valid admin roles (case-insensitive)
      // const adminRoles = ['admin', 'owner', 'moderator', 'administrator'];
      // const isAdminUser =

      // if (!isAdminUser) {
      //   logger.info(
      //     `User ${message.entityId} is not an admin, rejecting action. Role: ${userRole}`
      //   );
      //   // We'll handle the message in the handler
      //   state.data.isAdmin = false;
      //   return true; // Still return true so handler can show the error message
      // }

      // // Validate Discord configuration
      // if (!validation.isValid) {
      //   logger.error(`Discord validation failed: ${validation.error}`);
      //   return false;
      // }

      // Ensure Discord client is available

      // Get Discord client first

        // Try to proceed anyway or handle gracefully

      // Check if Discord connection is established

      // Get room and server ID

      // Fetch all channels from the server

            // Define textChannels property if it doesn't exist

      // Rest of your existing handler code...

      // Extract check-in details from user message

      // Check if report channel config exists for this server

      // Add table name to getMemories call

      // Get message source

      // Check if user is asking a question rather than providing check-in details

          // First ask for the report channel configuration

          // Ask for check-in schedule details

      // Continue with parsing check-in details

        // Parse the response

      // Get channel IDs from the parsed configuration

      // Find channel ID for updates based on channel name

      // Find channel ID for check-ins based on channel name

      // Update the config with the channel IDs

      // TODO : after things are parsed now store the check in form for group
      // TODO : store the check in storage

          // Extract source information

            // TODO : have to fetch server name

          // First create the room to avoid foreign key constraint error
            // Continue even if room creation fails - it might already exist

      // Store check-in schedule

        // Extract source information from the message or room

          // teamMemberUserName: checkInConfig.userDisplayName,

        // Use the same roomId as above to avoid foreign key constraint error

        // Ensure the room exists before storing the memory
          // Continue even if room creation fails - it might already exist

      // Send success message to the user

} // namespace elizaos
