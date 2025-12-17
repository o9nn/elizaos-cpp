#pragma once
#include <algorithm>
#include <any>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "elizaos/core.hpp"
#include "lib/base-client.hpp"
#include "types/base.hpp"
#include "types/messaging.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



class MessagingService {
public:
    std::future<Message> submitMessage(MessageSubmitParams params);
    Promise< completeMessage(MessageCompleteParams params);
    Promise< ingestExternalMessages(ExternalMessageParams params);
    std::future<MessageChannel> createChannel(ChannelCreateParams params);
    std::future<MessageChannel> createGroupChannel(GroupChannelCreateParams params);
    std::future<MessageChannel> getOrCreateDmChannel(DmChannelParams params);
    std::future<MessageChannel> getChannelDetails(UUID channelId);
    Promise< getChannelParticipants(UUID channelId);
    Promise< addAgentToChannel(UUID channelId, UUID agentId);
    Promise< removeAgentFromChannel(UUID channelId, UUID agentId);
    Promise< deleteChannel(UUID channelId);
    Promise< clearChannelHistory(UUID channelId);
    std::future<Message> postMessage(UUID channelId, const std::string& content, std::optional<std::unordered_map<std::string, std::any>> metadata);
    Promise< getChannelMessages(UUID channelId, std::optional<PaginationParams> params);
    std::future<Message> getMessage(UUID messageId);
    Promise< deleteMessage(UUID channelId, UUID messageId);
    std::future<Message> updateMessage(UUID messageId, const std::string& content);
    Promise< searchMessages(MessageSearchParams params);
    Promise< listServers();
    Promise< getServerChannels(UUID serverId);
    std::future<MessageServer> createServer(ServerCreateParams params);
    Promise< syncServerChannels(UUID serverId, ServerSyncParams params);
    Promise< deleteServer(UUID serverId);
    Promise< updateChannel(UUID channelId, ChannelUpdateParams params);
    Promise< generateChannelTitle(UUID channelId, UUID agentId);
    Promise< addUserToChannel(UUID channelId, UUID userId);
    Promise< addUsersToChannel(UUID channelId, const std::vector<UUID>& userIds);
    Promise< removeUserFromChannel(UUID channelId, UUID userId);
};


} // namespace elizaos
