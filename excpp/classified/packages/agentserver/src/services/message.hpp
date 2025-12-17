#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".bus.hpp"
#include ".types.hpp"
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use


 // Import the bus

// This interface defines the structure of messages coming from the server
struct MessageServiceMessage {
    UUID id;
    UUID channelId;
    UUID serverId;
    UUID authorId;
    std::optional<std::string> authorDisplayName;
    std::string content;
    std::optional<unknown> rawMessage;
    std::optional<std::string> sourceId;
    std::optional<std::string> sourceType;
    std::optional<UUID> inReplyToMessageId;
    double createdAt;
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
    void handleServerAgentUpdate(unknown data);
    std::future<bool> validateServerSubscription(MessageServiceMessage message);
    std::future<bool> validateNotSelfMessage(MessageServiceMessage message);
    Promise< ensureWorldAndRoomExist(MessageServiceMessage message);
    void catch(unknown error);
    void catch(unknown error);
    std::future<UUID> ensureAuthorEntityExists(MessageServiceMessage message);
    Memory createAgentMemory(MessageServiceMessage message, UUID agentAuthorEntityId, UUID agentRoomId, UUID agentWorldId);
    void handleIncomingMessage(unknown messageData);
    void handleMessageDeleted(unknown data);
    void handleChannelCleared(unknown data);
    void sendAgentResponseToBus(UUID agentRoomId, UUID agentWorldId, Content content, std::optional<UUID> inReplyToAgentMemoryId, std::optional<MessageServiceMessage> originalMessage);
    void notifyMessageComplete(std::optional<UUID> channelId, std::optional<UUID> serverId);
    std::unordered_map<std::string, std::string> getAuthHeaders();
    std::string getCentralMessageServerUrl();
    std::future<void> stop();
};

// Minimal plugin definition to register the service

} // namespace elizaos
