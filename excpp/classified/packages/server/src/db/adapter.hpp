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
#include "connection.hpp"
#include "elizaos/core.hpp"
#include "schema.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



class MessageDatabaseAdapter {
public:
    void db() const;
    std::future<MessageServer> createMessageServer(Omit<MessageServer data, auto 'id' | 'createdAt' | 'updatedAt'>);
    std::future<std::vector<MessageServer>> getMessageServers();
    std::variant<Promise<MessageServer, null>> getMessageServerById(UUID serverId);
    std::future<MessageChannel> createChannel(Omit<MessageChannel data, std::optional<UUID }> 'id' | 'createdAt' | 'updatedAt'> & { id, std::optional<std::vector<UUID>> participantIds);
    std::future<void> addChannelParticipants(UUID channelId, const std::vector<UUID>& userIds);
    std::future<MessageChannel> updateChannel(UUID channelId, std::optional<std::any> updates);
    std::future<void> deleteChannel(UUID channelId);
    std::future<MessageChannel> findOrCreateDmChannel(UUID user1Id, UUID user2Id, UUID messageServerId);
    std::future<CentralRootMessage> createMessage(Omit<CentralRootMessage data, auto 'id' | 'createdAt' | 'updatedAt'>);
    std::future<void> deleteMessage(UUID messageId);
    std::future<void> addAgentToServer(UUID serverId, UUID agentId);
    std::future<void> removeAgentFromServer(UUID serverId, UUID agentId);
    std::future<std::any> execute(const std::any& query);
    std::future<void> close();


} // namespace elizaos
