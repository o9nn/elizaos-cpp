#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include "api/index.hpp"
#include "api/system/environment.hpp"
#include "authMiddleware.hpp"
#include "bus.hpp"
#include "database/ServerDatabaseAdapter.hpp"
#include "database/ServerMigrationService.hpp"
#include "elizaos/core.hpp"
#include "elizaos/plugin-sql.hpp"
#include "elizaos/plugin-todo.hpp"
#include "game-api-plugin.hpp"
#include "loader.hpp"
#include "services/message.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// CRITICAL: Ensure http module is available globally before importing ws
// This fixes Bun bundling issues with the ws module

  // Ensure STATUS_CODES is available - this is needed for WebSocket compatibility
    // Just ensure the http module has what it needs without full replacement

  // Override require to ensure http is always available
    // Ignore if Module is not available

/**
 * Expands a file path starting with `~` to the project directory.
 *
 * @param filepath - The path to expand.
 * @returns The expanded path.
 */
std::string expandTildePath(const std::string& filepath);

std::string resolvePgliteDir(std::optional<std::string> dir, std::optional<std::string> fallbackDir);

/**
 * Represents a function that acts as a server middleware.
 * @param {express.Request} req - The request object.
 * @param {express.Response} res - The response object.
 * @param {express.NextFunction} next - The next function to be called in the middleware chain.
 * @returns {void}
 */
using ServerMiddleware = std::function<void(express::Request, express::Response, express::NextFunction)>;

// Define a type for Express errors

// Define WebSocket message data interface
struct WebSocketMessageData {
    std::string channelId;
    std::string agentId;
    std::string content;
    std::string author;
};

// Define log entry interface
struct LogEntry {
    std::string timestamp;
    std::string level;
    std::string message;
    std::optional<std::string> agentName;
};

/**
 * Class representing an agent server.
 */ /**
 * Represents an agent server which handles agents, database, and server functionalities.
 */
class AgentServer {
public:
    AgentServer();
    std::future<void> initialize(const std::string& postgresUrl);
    std::future<void> ensureDefaultServer();
    void initializeServer();
    void setupMessageBusListener();
    void setupWebSocketServer(const std::variant<http::Server, https::Server>& server);
    std::future<void> processWebSocketMessage(WebSocketMessageData messageData, WebSocket ws);
    void broadcastToWebSocketClients(Record<string message, auto unknown>, std::optional<std::string> _channelId, std::optional<std::string> _excludeClientId);
    void setupLogStreaming();
    void broadcastLogToSubscribers(LogEntry logEntry);
    void registerAgent(IAgentRuntime runtime);
    void unregisterAgent(UUID agentId);
    void registerMiddleware(ServerMiddleware middleware);
    std::future<void> start(double port);
    std::future<void> stop();
    std::future<MessageServer> createServer(Omit<MessageServer data, auto 'id' | 'createdAt' | 'updatedAt'>);
    std::future<std::vector<MessageServer>> getServers();
    std::variant<Promise<MessageServer, null>> getServerById(UUID serverId);
    std::variant<Promise<MessageServer, null>> getServerBySourceType(const std::string& sourceType);
    std::future<MessageChannel> createChannel(Omit<MessageChannel data, std::optional<UUID }> 'id' | 'createdAt' | 'updatedAt'> & { id, std::optional<std::vector<UUID>> participantIds);
    std::future<void> addParticipantsToChannel(UUID channelId, const std::vector<UUID>& userIds);
    std::future<void> deleteMessage(UUID messageId);
    std::future<void> deleteChannel(UUID channelId);
    std::future<void> clearChannelMessages(UUID channelId);
    std::future<MessageChannel> findOrCreateCentralDmChannel(UUID user1Id, UUID user2Id, UUID serverId);
    std::future<CentralRootMessage> createMessage(Omit<CentralRootMessage data, auto 'id' | 'createdAt' | 'updatedAt'>);
    std::future<void> removeParticipantFromChannel();
    std::future<void> addAgentToServer(UUID serverId, UUID agentId);
    std::future<void> removeAgentFromServer(UUID serverId, UUID agentId);
    void registerSignalHandlers();

// Export loader utilities

// Export types
* from './types';

} // namespace elizaos
