#pragma once
#include <algorithm>
#include <any>
#include <chrono>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>
#include "elizaos/core.hpp"
#include "schema.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct MessageServer {
    UUID id;
    std::string name;
    std::string sourceType;
    std::optional<std::string> sourceId;
    std::chrono::system_clock::time_point createdAt;
    std::chrono::system_clock::time_point updatedAt;
};

struct Channel {
    std::string id;
    UUID serverId;
    std::string name;
    std::string type;
    std::optional<std::string> sourceType;
    std::optional<std::string> sourceId;
    std::optional<std::string> topic;
    std::chrono::system_clock::time_point createdAt;
    std::chrono::system_clock::time_point updatedAt;
};

struct Message {
    std::string id;
    std::string channelId;
    std::string authorId;
    std::string content;
    std::optional<std::any> rawMessage;
    std::optional<std::string> sourceType;
    std::optional<std::string> sourceId;
    std::optional<std::string> inReplyToRootMessageId;
    std::chrono::system_clock::time_point createdAt;
    std::chrono::system_clock::time_point updatedAt;
};

// Define the minimal database type we need
using DrizzleDatabase = std::function<void()>;

/**
 * ServerDatabaseAdapter handles server-specific database operations
 * This adapter manages message servers, channels, and messaging infrastructure
 * that is specific to the server implementation and not part of the core agent functionality
 */
class ServerDatabaseAdapter {
public:
    ServerDatabaseAdapter();
    std::future<MessageServer> createMessageServer(std::optional<std::any> data);
    std::future<std::vector<MessageServer>> getMessageServers();
    std::variant<Promise<MessageServer, null>> getMessageServerById(UUID serverId);
    std::future<Channel> createChannel(std::optional<std::any> data);
    std::future<std::vector<Channel>> getChannelsForServer(UUID serverId);
    std::future<void> addAgentToServer(UUID serverId, UUID agentId);
    std::future<std::vector<UUID>> getAgentsForServer(UUID serverId);
    std::future<void> removeAgentFromServer(UUID serverId, UUID agentId);
    std::future<Message> createMessage(std::optional<std::any> data);
    std::future<std::vector<Message>> getMessagesForChannel(const std::string& channelId, double limit = 50, double offset = 0);
    std::future<void> addParticipantToChannel(const std::string& channelId, UUID userId);
    std::future<std::vector<UUID>> getParticipantsForChannel(const std::string& channelId);
    std::future<void> removeParticipantFromChannel(const std::string& channelId, UUID userId);
    std::future<Channel> findOrCreateDmChannel(UUID serverId, UUID entity1Id, UUID entity2Id);
    std::variant<Promise<Channel, null>> getChannelDetails(const std::string& channelId);
    std::future<Channel> updateChannel(const std::string& channelId, std::optional<std::any> updates);
    std::future<void> deleteChannel(const std::string& channelId);
    std::future<void> deleteMessage(const std::string& messageId);
};


} // namespace elizaos
