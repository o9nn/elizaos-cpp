#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".forms/checkInForm.hpp"
#include ".forms/scheduleForm.hpp"
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct DiscordComponentInteraction {
    std::string customId;
    double componentType;
    std::optional<std::vector<std::string>> values;
};

struct TextChannel {
    std::string id;
    std::string name;
    std::string type;
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
 * @returns {Promise<any>} The Discord client
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

      // Check if report channel config exists for this server

      // Add table name to getMemories call

      // if (!existingConfig) {
      //   logger.info('No existing report channel config found - sending report form first');
      //   logger.info('Using server ID:', serverId);
      //     serverId,
      //     // serverName: String(message.content.serverName),
      //   });
      // } else {
      //   logger.info('Found existing report channel config:', existingConfig);
      //   logger.info(`Sending schedule form with ${textChannels.length} channels`);
      // }

      // Send message to user asking for check-in details

        // First ask for the report channel configuration

        // Ask for check-in schedule details


} // namespace elizaos
