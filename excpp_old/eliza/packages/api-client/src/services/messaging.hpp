#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include ".lib/base-client.hpp"
#include ".types/base.hpp"
#include ".types/messaging.hpp"
#include "elizaos/core.hpp"

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
    std::future<Message> postMessage(UUID channelId, const std::string& content, std::optional<Record<string> metadata, auto any>);
    Promise< getChannelMessages(UUID channelId, std::optional<std::variant<PaginationParams & { before: Date, string; after: Date, string }>> params);
    std::future<Message> getMessage(UUID messageId);
    std::future<Message> updateMessage(UUID messageId, const std::string& content);
    std::future<MessageServer> createServer(ServerCreateParams params);
    void for(auto const userId of userIds);


} // namespace elizaos
