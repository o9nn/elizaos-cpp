#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_OTAKU_SRC_PACKAGES_API-CLIENT_SRC_SERVICES_MESSAGING_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_OTAKU_SRC_PACKAGES_API-CLIENT_SRC_SERVICES_MESSAGING_H
#include "core.h"
#include "@elizaos/core.h"
#include "../lib/base-client.h"
#include "../types/messaging.h"
#include "../types/base.h"
#include "../types/jobs.h"

class ChannelCreatePayload;
class GroupChannelCreatePayload;
class DmChannelQuery;
class MessagingService;

class ChannelCreatePayload : public object, public std::enable_shared_from_this<ChannelCreatePayload> {
public:
    using std::enable_shared_from_this<ChannelCreatePayload>::shared_from_this;
    string name;

    std::shared_ptr<ChannelType> type;

    std::shared_ptr<UUID> server_id;

    std::shared_ptr<ChannelMetadata> metadata;
};

class GroupChannelCreatePayload : public object, public std::enable_shared_from_this<GroupChannelCreatePayload> {
public:
    using std::enable_shared_from_this<GroupChannelCreatePayload>::shared_from_this;
    string name;

    std::shared_ptr<UUID> server_id;

    array<std::shared_ptr<UUID>> participantCentralUserIds;

    std::shared_ptr<ChannelType> type;

    std::shared_ptr<ChannelMetadata> metadata;
};

class DmChannelQuery : public object, public std::enable_shared_from_this<DmChannelQuery> {
public:
    using std::enable_shared_from_this<DmChannelQuery>::shared_from_this;
    std::shared_ptr<UUID> currentUserId;

    std::shared_ptr<UUID> targetUserId;

    std::shared_ptr<UUID> dmServerId;
};

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
    virtual std::shared_ptr<Promise<object>> addAgentToServer(std::shared_ptr<UUID> serverId, std::shared_ptr<UUID> agentId);
    virtual std::shared_ptr<Promise<object>> removeAgentFromServer(std::shared_ptr<UUID> serverId, std::shared_ptr<UUID> agentId);
    virtual std::shared_ptr<Promise<std::shared_ptr<Message>>> postMessage(std::shared_ptr<UUID> channelId, string content, std::shared_ptr<MessageMetadata> metadata = undefined);
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
    virtual std::shared_ptr<Promise<object>> generateChannelTitle(string userMessage, std::shared_ptr<UUID> agentId);
    virtual std::shared_ptr<Promise<object>> generateChannelPrompts(std::shared_ptr<UUID> channelId, std::shared_ptr<UUID> agentId, double count = 4);
    virtual std::shared_ptr<Promise<object>> addUserToChannel(std::shared_ptr<UUID> channelId, std::shared_ptr<UUID> userId);
    virtual std::shared_ptr<Promise<object>> addUsersToChannel(std::shared_ptr<UUID> channelId, array<std::shared_ptr<UUID>> userIds);
    virtual std::shared_ptr<Promise<object>> removeUserFromChannel(std::shared_ptr<UUID> channelId, std::shared_ptr<UUID> userId);
    virtual std::shared_ptr<Promise<std::shared_ptr<CreateJobResponse>>> createJob(std::shared_ptr<CreateJobRequest> params);
    virtual std::shared_ptr<Promise<std::shared_ptr<JobDetailsResponse>>> getJob(string jobId);
    virtual std::shared_ptr<Promise<std::shared_ptr<JobListResponse>>> listJobs(std::shared_ptr<ListJobsParams> params = undefined);
    virtual std::shared_ptr<Promise<std::shared_ptr<JobHealthResponse>>> getJobsHealth();
    virtual std::shared_ptr<Promise<std::shared_ptr<JobDetailsResponse>>> pollJob(string jobId, double interval = 1000, double maxAttempts = 30);
    virtual std::shared_ptr<Promise<std::shared_ptr<JobDetailsResponse>>> createAndWaitForJob(std::shared_ptr<CreateJobRequest> params, double pollInterval = 1000, double maxAttempts = 30);
    MessagingService(std::shared_ptr<ApiClientConfig> config);
};

#endif
