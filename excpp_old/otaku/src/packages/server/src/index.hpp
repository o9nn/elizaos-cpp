#pragma once
#include <any>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include "api/index.js.hpp"
#include "api/system/environment.js.hpp"
#include "bus.js.hpp"
#include "elizaos/core.hpp"
#include "loader.js.hpp"
#include "middleware/index.js.hpp"
#include "services/message.js.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



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

/**
 * Interface for defining server configuration options.
 * @typedef {Object} ServerOptions
 * @property {ServerMiddleware[]} [middlewares] - Optional array of server middlewares.
 * @property {string} [dataDir] - Optional directory for storing server data.
 * @property {string} [postgresUrl] - Optional URL for connecting to a PostgreSQL database.
 */
struct ServerOptions {
    std::optional<std::vector<ServerMiddleware>> middlewares;
    std::optional<std::string> dataDir;
    std::optional<std::string> postgresUrl;
    std::optional<std::string> clientPath;
};

/**
 * Determines if the web UI should be enabled based on environment variables.
 *
 * @returns {boolean} - Returns true if UI should be enabled, false otherwise
 */
bool isWebUIEnabled();

/**
 * Class representing an agent server.
 */ /**
 * Represents an agent server which handles agents, database, and server functionalities.
 */
class AgentServer {
public:
    AgentServer();
    void if(auto !this.elizaOS);
    void for(auto const id of agentIds);
    std::future<void> stopAgents(const std::vector<UUID>& agentIds);
    std::vector<IAgentRuntime> getAllAgents();
    IAgentRuntime getAgent(UUID agentId);
    std::future<void> initialize(std::optional<ServerOptions> options);
    std::future<void> ensureDefaultServer();
    void initializeServer(std::optional<ServerOptions> options);
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
    std::future<MessageChannel> updateChannel(UUID channelId, std::optional<std::any> updates);
    std::future<void> deleteChannel(UUID channelId);
    std::future<void> clearChannelMessages(UUID channelId);
    std::future<MessageChannel> findOrCreateCentralDmChannel(UUID user1Id, UUID user2Id, UUID messageServerId);
    std::future<CentralRootMessage> createMessage(Omit<CentralRootMessage data, auto 'id' | 'createdAt' | 'updatedAt'>);
    std::future<void> removeParticipantFromChannel();
    std::future<void> addAgentToServer(UUID serverId, UUID agentId);
    std::future<void> removeAgentFromServer(UUID serverId, UUID agentId);
    void registerSignalHandlers();

// Export loader utilities

// Export types
* from './types';

// Export ElizaOS from core (re-for convenience)

} // namespace elizaos
