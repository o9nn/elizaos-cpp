#include ".bus.hpp"
#include ".types.hpp"
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


 // Import the bus

// This interface defines the structure of messages coming from the server
struct MessageServiceMessage {
    UUID; // root_message.id id;
    UUID channelId;
    UUID serverId;
    UUID; // UUID of a central user identity authorId;
    std::optional<string; // Display name from central user identity> authorDisplayName;
    std::string content;
    std::optional<unknown> rawMessage;
    std::optional<string; // original platform message ID> sourceId;
    std::optional<std::string> sourceType;
    std::optional<UUID> inReplyToMessageId;
    double createdAt;
    std::optional<std::unordered_map<std::string, unknown>> metadata;
};

class MessageBusService extends Service {
  static serviceType = AgentServerServiceType.MESSAGE_BUS_SERVICE;
  // serviceName will be automatically derived as "MessageBus" from class name

  capabilityDescription = 'Manages connection and message synchronization with the message server.';
  private static instances: Map<string, MessageBusService> = new Map();

  private boundHandleIncomingMessage: (data: unknown) => void;
  private boundHandleServerAgentUpdate: (data: unknown) => void;
  private boundHandleMessageDeleted: (data: unknown) => void;
  private boundHandleChannelCleared: (data: unknown) => void;
  private subscribedServers: Set<UUID> = new Set();
  private processedMessages: Set<string> = new Set();

  constructor(runtime: IAgentRuntime) {
    super(runtime);
    this.boundHandleIncomingMessage = this.handleIncomingMessage.bind(this);
    this.boundHandleServerAgentUpdate = this.handleServerAgentUpdate.bind(this);
    this.boundHandleMessageDeleted = this.handleMessageDeleted.bind(this);
    this.boundHandleChannelCleared = this.handleChannelCleared.bind(this);
    // Don't connect here - let start() handle it
  }

    // Check if instance already exists for this agent

    // Remove any existing listeners before adding new ones

    // Now add the listeners

    // Delay initial fetch to allow server to start up properly
    // This prevents circular dependency during startup
        // Initialize by fetching servers this agent belongs to
        // Then fetch valid channels for those servers
        // Set up default server and continue - fetches will retry later

      // Clear existing channel IDs before fetching new ones

      // Include the default server ID if not already in subscribed servers

      // Fetch channels for each subscribed server
          // Use URL constructor for safe URL building
              // Add channel IDs to the set

      // First check if channel is in our cached set
        // Try to verify the channel exists by fetching its details
        // Use URL constructor for safe URL building

          // Channel exists, add it to our valid set for future use

      // Now fetch the participants
      // Use URL constructor for safe URL building

      // Use URL constructor for safe URL building

          // Always include the default server
        // Even if the request fails, ensure we're subscribed to the default server
      // Even on error, ensure we're subscribed to the default server

      // Refresh channel IDs to include channels from the new server
      // Refresh channel IDs to remove channels from the removed server

    // Handle both underscore and camelCase versions

    // Check if we've already processed this message

    // Mark message as processed

    // Clean up old processed messages to prevent memory leak
      // Keep only the last 500 messages

    // Transform the channel_id to the agent-specific room ID
    // Handle both channel_id (underscore) and channelId (camelCase) for compatibility

    // Check if the agent is a participant in the transformed room ID

        // Also check the original channel participants as a fallback
          // return;
      // If room doesn't exist yet, we'll create it below

      // Check if this memory already exists (in case of duplicate processing)

        // Check if this is just a processing notification, not an actual response

        // Send response to central bus

        // The core runtime/bootstrap plugin will handle creating the agent's own memory of its response.
        // So, we return an empty array here as this callback's primary job is to ferry the response externally.

      // Convert the central message ID to the agent's unique memory ID

      // Try to find and delete the existing memory

        // Emit MESSAGE_DELETED event with the existing memory

      // Convert the central channel ID to the agent's unique room ID

      // Get all memories for this room and emit deletion events for each

      // Emit CHANNEL_CLEARED event to bootstrap which will handle bulk deletion

    // If agent decides to IGNORE or has no valid text, notify completion and skip sending response

    // Resolve reply-to message ID from agent memory metadata

    // Actual fetch to the central server API
    // Use URL constructor for safe URL building

      // Response was successful, also broadcast via WebSocket
      // Get the server instance to access WebSocket broadcast

    // Add authentication header if ELIZA_SERVER_AUTH_TOKEN is configured

    // Validate and sanitize server port

    // On Windows, use 127.0.0.1 instead of localhost to avoid resolution issues

    // Strict validation to prevent SSRF attacks

      // Only allow HTTP/HTTPS protocols

      // Only allow safe localhost variants and block private/internal IPs

      // Validate port range

      // Remove any potentially dangerous URL components

    // Clear processed messages

// Minimal plugin definition to register the service

} // namespace elizaos
