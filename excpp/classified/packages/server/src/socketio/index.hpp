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



class SocketIORouter {
  private agents: Map<UUID, IAgentRuntime>;
  private connections: Map<string, UUID>; // socket.id -> agentId (for agent-specific interactions like log streaming, if any)
  private logStreamConnections: Map<string, { agentName?: string; level?: string }>;

    // Emit ENTITY_JOINED event for bootstrap plugin to handle world/entity creation

      // Get the first available runtime (there should typically be one)

    // Special handling for default server ID "0"

      // Check if this is a DM channel and emit ENTITY_JOINED for proper world setup

      // Ensure the channel exists before creating the message

        // Auto-create the channel if it doesn't exist
          // First verify the server exists

          // Determine if this is likely a DM based on the context

          // For DM channels, we need to determine the participants
            // Try to extract the other participant from metadata or payload

      // Immediately broadcast the message to all clients in the channel

      // Broadcast to everyone in the channel except the sender

      // Also send back to the sender with the server-assigned ID


} // namespace elizaos
