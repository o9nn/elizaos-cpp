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



// Use Next.js API proxy to avoid CORS issues

// Optional API key for authentication

// ElizaOS API Response Types

// Backend Memory type from ElizaOS
struct BackendMemory {
    std: id;
    std:; // UUID of sender (user or agent) entityId;
    std:; // UUID of the agent this memory belongs to agentId;
    std:; // UUID of the room roomId;
    { content;
    std::optional<std:> text;
    std::optional<std:> source;
    std::optional<std:> thought;
    std::optional<std::vector<std::string>> actions;
    std::optional<std::vector<std::string>> attachments;
    std::optional<{> metadata;
    std::optional<std:> entityName;
    double createdAt;
    std::optional<std:> worldId;
    std::optional<double> importance;
};

// Agent information
struct Agent {
    std: id;
    std: name;
    std::optional<std:> bio;
    std::optional<std::unordered_map<std:, std:>> settings;
    std::optional<"active" | "inactive"> status;
};

// Room information
struct Room {
    std: id;
    std: name;
    "dm" | "group" | "channel" type;
    std: agentId;
    std::optional<std:> worldId;
    std::optional<std:> serverId;
    std::optional<std::unordered_map<std:, std:>> metadata;
};

// Channel information
struct Channel {
    std: id;
    std: name;
    std: serverId;
    std::optional<std:> description;
    std::optional<std::vector<std::string>> participants;
    std::optional<std::vector<std::string>> agents;
};

// Message submission payload
struct MessageSubmission {
    std: message;
    std: agentId;
    std::optional<std:> roomId;
    std::optional<std:> channelId;
    std::optional<std:> serverId;
    std::optional<std:> senderId;
    std::optional<std:> senderName;
    std::optional<std:> source;
    std::optional<std::vector<std::string>> attachments;
    std::optional<std::unordered_map<std:, std:>> metadata;
};

/**
 * Enhanced fetch wrapper with API key authentication
 */

  // Prepare headers with std::optional API key

    // Check content type before parsing

// === AGENT MANAGEMENT ===

/**
 * Get all available agents
 */

/**
 * Get specific agent details
 */

// === MEMORY MANAGEMENT ===

/**
 * Fetches memories for a given agent (latest API)
 */

      fetcher(url);

    return transformMemoriesToChatMessages(memories);

/**
 * Fetches memories for a specific room (latest API)
 */

      fetcher(url);

    return transformMemoriesToChatMessages(memories);

/**
 * Transform backend memories to frontend chat messages
 */
std::vector<ChatMessage> transformMemoriesToChatMessages(const std::vector<BackendMemory>& memories);

// === MESSAGING SYSTEM ===

/**
 * Submit a message via the messaging system
 */

/**
 * Get channel messages
 */

    // Get agent ID from environment for proper agent detection

    // Transform channel messages to match the UI format exactly
        // More accurate agent detection using agent ID

/**
 * Send message to channel
 */

// === ROOM MANAGEMENT ===

/**
 * Get agent rooms
 */

/**
 * Create a new room
 */

// === SERVER HEALTH ===

/**
 * Ping server for health check
 */
    // ElizaOS returns {pong: true, timestamp} for successful ping

/**
 * Get server status
 */

// === DM CHANNEL MANAGEMENT ===

struct DMChannel {
    std: id;
    std: name;
    "DM" type;
    { metadata;
    std: user1;
    std: user2;
    std: forAgent;
    std: createdAt;
    std::optional<std:> sessionId;
    std::optional<std:> title;
    std::vector<std::string> participants;
};

/**
 * Create a new DM channel
 */

/**
 * Get or create a DM channel for a session
 */

/**
 * List DM channels for a user and agent
 */


} // namespace elizaos
