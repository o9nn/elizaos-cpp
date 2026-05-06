#pragma once
#include <any>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
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
    Date createdAt;
    Date updatedAt;
};

struct Channel {
    std::string id;
    UUID serverId;
    std::string name;
    std::string type;
    std::optional<std::string> sourceType;
    std::optional<std::string> sourceId;
    std::optional<std::string> topic;
    Date createdAt;
    Date updatedAt;
};

struct Message {
    std::string id;
    std::string channelId;
    std::string authorId;
    std::string content;
    std::optional<unknown> rawMessage;
    std::optional<std::string> sourceType;
    std::optional<std::string> sourceId;
    std::optional<std::string> inReplyToRootMessageId;
    Date createdAt;
    Date updatedAt;
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
    ServerDatabaseAdapter(DrizzleDatabase private db);
    std::future<MessageServer> createMessageServer(std::optional<{
    id: UUID;
    name: string;
    sourceType: string;
    sourceId: string;
    metadata: Record<string> data, auto unknown>;
  });
    std::future<void> addAgentToServer(UUID serverId, UUID agentId);
    std::future<void> removeAgentFromServer(UUID serverId, UUID agentId);
    std::future<void> addParticipantToChannel(const std::string& channelId, UUID userId);
    std::future<void> removeParticipantFromChannel(const std::string& channelId, UUID userId);
    std::future<Channel> findOrCreateDmChannel(UUID serverId, UUID entity1Id, UUID entity2Id);
    std::future<Channel> updateChannel(const std::string& channelId, std::optional<std::any> updates);
    std::future<void> deleteChannel(const std::string& channelId);
    std::future<void> deleteMessage(const std::string& messageId);


} // namespace elizaos
