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



// Extension of CheckInSchedule with additional fields

// Interface for storing report channel configuration
struct ReportChannelConfig {
    std::optional<string; // Made optional> serverId;
    std::string serverName;
    std::string channelId;
    std::string createdAt;
};

using BaseInteraction = std::variant<ButtonInteraction, StringSelectMenuInteraction, SelectMenuInteraction>;

// Define our custom interaction type
struct ExtendedInteraction {
    std::string customId;
    std::optional<User> user;
    std::optional<{ user?: { id: string } }> member;

class CheckInService extends Service {
  private formSelections: Map<string, Record<string, string[]>> = new Map();
  private reportChannelConfigs: Map<string, ReportChannelConfig> = new Map(); // Store report channel configs by server ID
  static serviceType = 'CHECKIN_SERVICE';
  capabilityDescription = 'Manages team member check-in schedules';

  constructor(protected runtime: IAgentRuntime) {
    super(runtime);
  }

    // Cleanup if needed

    // Listen for Discord interactions

        // Check if this is a button interaction

        // Check if this is a modal submit

      // Cast to DiscordService after null check

      // Log what's in the service to see its structure

      // Check if client exists and is ready

      // TODO : get discord service cool or in start

      // Fetch user details

      // Create check-in schedule

      // Store the schedule

      // Send confirmation message

        // Check if this is a duplicate key error

      // First create the room if it doesn't exist

  // New methods for handling report channel configuration

      // Parse server info from form data

      // Ensure the room exists before trying to access it

  // Helper method to get report channel for a server

} // namespace elizaos
