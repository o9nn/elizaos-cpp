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



// WebSocket message interfaces
struct WebSocketConnectMessage {
    std::optional<std::string> agent_id;
    std::optional<std::string> channel_id;
};

struct WebSocketChannelJoinMessage {
    std::optional<std::string> roomId;
    std::optional<std::string> agentId;
};

struct WebSocketUserMessage {
    std::string content;
    std::optional<std::string> author;
    std::optional<std::string> channel_id;
    std::optional<std::string> agent_id;
    std::optional<double> timestamp;
};

struct WebSocketSendMessage {
    std::string content;
    std::optional<std::string> senderName;
    std::optional<std::string> author;
    std::optional<std::string> roomId;
    std::optional<std::string> channelId;
    std::optional<std::string> agentId;
    std::optional<double> timestamp;
};

struct ProcessAgentMessageData {
    std::string content;
    std::string author;
    std::optional<std::string> channel_id;
    std::optional<std::string> agent_id;
    double timestamp;
};

struct WebSocketOutgoingMessage {
    std::string type;
};

class WebSocketRouter {
  private agents: Map<UUID, IAgentRuntime>;
  private connections: Map<WebSocket, { agentId?: UUID; channelId?: string }>;

    // Initialize connection metadata

    // Send connection confirmation

    // Send ping every 30 seconds to keep connection alive

    // Update connection metadata

    // Update connection metadata

    // Get connection data or use provided IDs

    // Instead of processing directly through agent, use the message bus system

      // Create a message to route through the server's message ingestion system

      // Convert channel ID to UUID (use existing string or generate new UUID)

      // Create message in the database first

      // Emit to the internal message bus for agent processing

      // Import the message bus

      // Send acknowledgment back to WebSocket client

      // Also broadcast to other WebSocket clients watching this channel

  // Broadcast message to all connected clients for a specific agent/channel


} // namespace elizaos
