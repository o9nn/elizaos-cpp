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
#include ".bus.hpp"
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use


 // Import the bus

// This interface defines the structure of messages coming from the server
struct MessageServiceMessage {
    UUID id;
    UUID channel_id;
    UUID server_id;
    UUID author_id;
    std::optional<std::string> author_display_name;
    std::string content;
    std::optional<unknown> raw_message;
    std::optional<std::string> source_id;
    std::optional<std::string> source_type;
    std::optional<UUID> in_reply_to_message_id;
    double created_at;
    std::optional<MessageMetadata> metadata;
};

class MessageBusService {
public:
    MessageBusService(IAgentRuntime runtime);
    std::future<Service> start(IAgentRuntime runtime);
    std::future<void> stop(IAgentRuntime runtime);
    void connectToMessageBus();
    std::future<void> fetchValidChannelIds();
    std::future<std::vector<std::string>> getChannelParticipants(UUID channelId);
    void fetchAgentServers();
    void handleServerAgentUpdate(const std::any& data);
    std::future<bool> validateServerSubscription(MessageServiceMessage message);
    UUID resolveServerId(std::optional<std::variant<{ serverId: string, UUID }>> world, std::optional<std::variant<{ serverId: string, UUID }>> room, std::optional<MessageServiceMessage> originalMessage);
    std::future<bool> validateNotSelfMessage(MessageServiceMessage message);
    void catch(const std::any& error);
    void catch(const std::any& error);
    std::future<UUID> ensureAuthorEntityExists(MessageServiceMessage message);
    Memory createAgentMemory(MessageServiceMessage message, UUID agentAuthorEntityId, UUID agentRoomId, UUID agentWorldId);
    void handleIncomingMessage(unknown data);
    void handleMessageDeleted(const std::any& data);
    void handleChannelCleared(const std::any& data);
    void sendAgentResponseToBus(UUID agentRoomId, UUID agentWorldId, Content content, std::optional<UUID> inReplyToAgentMemoryId, std::optional<MessageServiceMessage> originalMessage);
    void sendErrorResponseToBus(UUID channelId, UUID serverId, const std::string& errorMessage);
    void notifyActionStart(UUID agentRoomId, UUID agentWorldId, Content content, UUID messageId, std::optional<UUID> inReplyToAgentMemoryId, std::optional<MessageServiceMessage> originalMessage);
    void notifyActionUpdate(UUID agentRoomId, UUID agentWorldId, Content content, UUID messageId, std::optional<UUID> inReplyToAgentMemoryId, std::optional<MessageServiceMessage> originalMessage);
    void notifyMessageComplete(std::optional<UUID> channelId, std::optional<UUID> serverId);
    std::unordered_map<std::string, std::string> getAuthHeaders();
    std::optional<std::string> generateAgentAuthToken();
    std::string getCentralMessageServerUrl();
    std::future<void> stop();

// Minimal plugin definition to register the service

} // namespace elizaos
