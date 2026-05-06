#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_API-CLIENT_SRC_SERVICES_MESSAGING_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_API-CLIENT_SRC_SERVICES_MESSAGING_H
#include "core.h"
#include "@elizaos/core.h"
#include "../lib/base-client.h"
#include "../types/messaging.h"
#include "../types/base.h"

class MessagingService;

class MessagingService : public BaseApiClient, public std::enable_shared_from_this<MessagingService> {
public:
    using std::enable_shared_from_this<MessagingService>::shared_from_this;
    virtual std::shared_ptr<Promise<std::shared_ptr<Message>>> submitMessage(std::shared_ptr<MessageSubmitParams> params);
    virtual std::shared_ptr<Promise<object>> completeMessage(std::shared_ptr<MessageCompleteParams> params);
    virtual std::shared_ptr<Promise<object>> ingestExternalMessages(std::shared_ptr<ExternalMessageParams> params);
    virtual std::shared_ptr<Promise<std::shared_ptr<MessageChannel>>> createChannel(std::shared_ptr<ChannelCreateParams> params);
    virtual std::shared_ptr<Promise<std::shared_ptr<MessageChannel>>> createGroupChannel(std::shared_ptr<GroupChannelCreateParams> params);
    virtual std::shared_ptr<Promise<std::shared_ptr<MessageChannel>>> getOrCreateDmChannel(std::shared_ptr<DmChannelParams> params);
    virtual std::shared_ptr<Promise<std::shared_ptr<MessageChannel>>> getChannelDetails(std::shared_ptr<UUID> channelId);
    virtual std::shared_ptr<Promise<object>> getChannelParticipants(std::shared_ptr<UUID> channelId);
    virtual std::shared_ptr<Promise<object>> addAgentToChannel(std::shared_ptr<UUID> channelId, std::shared_ptr<UUID> agentId);
    virtual std::shared_ptr<Promise<object>> removeAgentFromChannel(std::shared_ptr<UUID> channelId, std::shared_ptr<UUID> agentId);
    virtual std::shared_ptr<Promise<object>> deleteChannel(std::shared_ptr<UUID> channelId);
    virtual std::shared_ptr<Promise<object>> clearChannelHistory(std::shared_ptr<UUID> channelId);
    virtual std::shared_ptr<Promise<std::shared_ptr<Message>>> postMessage(std::shared_ptr<UUID> channelId, string content, Record<string, any> metadata = undefined);
    virtual std::shared_ptr<Promise<object>> getChannelMessages(std::shared_ptr<UUID> channelId, any params = undefined);
    virtual std::shared_ptr<Promise<std::shared_ptr<Message>>> getMessage(std::shared_ptr<UUID> messageId);
    virtual std::shared_ptr<Promise<object>> deleteMessage(std::shared_ptr<UUID> channelId, std::shared_ptr<UUID> messageId);
    virtual std::shared_ptr<Promise<std::shared_ptr<Message>>> updateMessage(std::shared_ptr<UUID> messageId, string content);
    virtual std::shared_ptr<Promise<object>> searchMessages(std::shared_ptr<MessageSearchParams> params);
    virtual std::shared_ptr<Promise<object>> listServers();
    virtual std::shared_ptr<Promise<object>> getServerChannels(std::shared_ptr<UUID> serverId);
    virtual std::shared_ptr<Promise<std::shared_ptr<MessageServer>>> createServer(std::shared_ptr<ServerCreateParams> params);
    virtual std::shared_ptr<Promise<object>> syncServerChannels(std::shared_ptr<UUID> serverId, std::shared_ptr<ServerSyncParams> params);
    virtual std::shared_ptr<Promise<object>> deleteServer(std::shared_ptr<UUID> serverId);
    virtual std::shared_ptr<Promise<object>> updateChannel(std::shared_ptr<UUID> channelId, std::shared_ptr<ChannelUpdateParams> params);
    virtual std::shared_ptr<Promise<object>> generateChannelTitle(std::shared_ptr<UUID> channelId, std::shared_ptr<UUID> agentId);
    virtual std::shared_ptr<Promise<object>> addUserToChannel(std::shared_ptr<UUID> channelId, std::shared_ptr<UUID> userId);
    virtual std::shared_ptr<Promise<object>> addUsersToChannel(std::shared_ptr<UUID> channelId, array<std::shared_ptr<UUID>> userIds);
    virtual std::shared_ptr<Promise<object>> removeUserFromChannel(std::shared_ptr<UUID> channelId, std::shared_ptr<UUID> userId);
    MessagingService(std::shared_ptr<ApiClientConfig> config);
};

#endif
