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
    std::string id;
    string; // UUID of sender (user or agent) entityId;
    string; // UUID of the agent this memory belongs to agentId;
    string; // UUID of the room roomId;
    { content;
    std::optional<std::string> text;
    std::optional<std::string> source;
    std::optional<std::string> thought;
    std::optional<std::vector<std::string>> actions;
    std::optional<std::vector<std::any>> attachments;
    std::optional<{> metadata;
    std::optional<std::string> entityName;
    double createdAt;
    std::optional<std::string> worldId;
    std::optional<double> importance;
};

// Agent information
struct Agent {
    std::string id;
    std::string name;
    std::optional<std::string> bio;
    std::optional<std::unordered_map<std::string, std::any>> settings;
    std::optional<"active" | "inactive"> status;
};

// Room information
struct Room {
    std::string id;
    std::string name;
    "dm" | "group" | "channel" type;
    std::string agentId;
    std::optional<std::string> worldId;
    std::optional<std::string> serverId;
    std::optional<std::unordered_map<std::string, std::any>> metadata;
};

// Channel information
struct Channel {
    std::string id;
    std::string name;
    std::string serverId;
    std::optional<std::string> description;
    std::optional<std::vector<std::string>> participants;
    std::optional<std::vector<std::string>> agents;
};

// Message submission payload
struct MessageSubmission {
    std::string message;
    std::string agentId;
    std::optional<std::string> roomId;
    std::optional<std::string> channelId;
    std::optional<std::string> serverId;
    std::optional<std::string> senderId;
    std::optional<std::string> senderName;
    std::optional<std::string> source;
    std::optional<std::vector<std::any>> attachments;
    std::optional<std::unordered_map<std::string, std::any>> metadata;
};

/**
 * Enhanced fetch wrapper with API key authentication
 */

  // Prepare headers with optional API key

    // Check content type before parsing

// ====== AGENT MANAGEMENT ======

/**
 * Get all available agents
 */

/**
 * Get specific agent details
 */

// ====== MEMORY MANAGEMENT ======

/**
 * Fetches memories for a given agent (latest API)
 */

      await fetcher(url);

    return transformMemoriesToChatMessages(memories);

/**
 * Fetches memories for a specific room (latest API)
 */

      await fetcher(url);

    return transformMemoriesToChatMessages(memories);

/**
 * Transform backend memories to frontend chat messages
 */
std::vector<ChatMessage> transformMemoriesToChatMessages(const std::vector<BackendMemory>& memories);

// ====== MESSAGING SYSTEM ======

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

// ====== ROOM MANAGEMENT ======

/**
 * Get agent rooms
 */

/**
 * Create a new room
 */

// ====== SERVER HEALTH ======

/**
 * Ping server for health check
 */
    // ElizaOS returns {pong: true, timestamp: number} for successful ping

/**
 * Get server status
 */

// ====== DM CHANNEL MANAGEMENT ======

struct DMChannel {
    std::string id;
    std::string name;
    "DM" type;
    { metadata;
    std::string user1;
    std::string user2;
    std::string forAgent;
    std::string createdAt;
    std::optional<std::string> sessionId;
    std::optional<std::string> title;
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
