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
    Promise< getChannelMessages(UUID channelId, std::optional<std::variant<PaginationParams & { before: Date, string; after: Date, string }>> params);
    std::future<Message> getMessage(UUID messageId);
    std::future<Message> updateMessage(UUID messageId, const std::string& content);
    std::future<MessageServer> createServer(ServerCreateParams params);
    void for(auto const userId of userIds);
    std::future<CreateJobResponse> createJob(CreateJobRequest params);
    std::future<JobDetailsResponse> getJob(const std::string& jobId);
    std::future<JobListResponse> listJobs(std::optional<ListJobsParams> params);
    std::future<JobHealthResponse> getJobsHealth();
    void catch(auto error);
    std::future<JobDetailsResponse> pollJob(const std::string& jobId, number = 1000 interval, number = 30 maxAttempts);
    std::future<JobDetailsResponse> createAndWaitForJob(CreateJobRequest params, number = 1000 pollInterval, number = 30 maxAttempts);


} // namespace elizaos
