#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".lib/base-client.hpp"
#include ".types/base.hpp"
#include ".types/jobs.hpp"
#include ".types/messaging.hpp"
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Internal payload interfaces for API requests
struct ChannelCreatePayload {
    std::string name;
    ChannelType type;
    UUID server_id;
    std::optional<ChannelMetadata> metadata;
};

struct GroupChannelCreatePayload {
    std::string name;
    UUID server_id;
    std::vector<UUID> participantCentralUserIds;
    std::optional<ChannelType> type;
    std::optional<ChannelMetadata> metadata;
};

struct DmChannelQuery {
    UUID currentUserId;
    UUID targetUserId;
    UUID dmServerId;
};

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
    Promise< addAgentToServer(UUID serverId, UUID agentId);
    Promise< removeAgentFromServer(UUID serverId, UUID agentId);
    std::future<Message> postMessage(UUID channelId, const std::string& content, std::optional<MessageMetadata> metadata);
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
    Promise< generateChannelTitle(const std::string& userMessage, UUID agentId);
    Promise< generateChannelPrompts(UUID channelId, UUID agentId, double count = 4);
    Promise< addUserToChannel(UUID channelId, UUID userId);
    Promise< addUsersToChannel(UUID channelId, const std::vector<UUID>& userIds);
    Promise< removeUserFromChannel(UUID channelId, UUID userId);
    std::future<CreateJobResponse> createJob(CreateJobRequest params);
    std::future<JobDetailsResponse> getJob(const std::string& jobId);
    std::future<JobListResponse> listJobs(std::optional<ListJobsParams> params);
    std::future<JobHealthResponse> getJobsHealth();
    std::future<JobDetailsResponse> pollJob(const std::string& jobId, double interval = 1000, double maxAttempts = 30);
    std::future<JobDetailsResponse> createAndWaitForJob(CreateJobRequest params, double pollInterval = 1000, double maxAttempts = 30);
};


} // namespace elizaos
