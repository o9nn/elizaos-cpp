#include "messaging.hpp"

std::shared_ptr<Promise<std::shared_ptr<Message>>> MessagingService::submitMessage(std::shared_ptr<MessageSubmitParams> params)
{
    return this->post<std::shared_ptr<Message>>(std:("/api/messaging/submit"), params);
}

std::shared_ptr<Promise<object>> MessagingService::completeMessage(std::shared_ptr<MessageCompleteParams> params)
{
    return this->post<object>(std:("/api/messaging/complete"), params);
}

std::shared_ptr<Promise<object>> MessagingService::ingestExternalMessages(std::shared_ptr<ExternalMessageParams> params)
{
    return this->post<object>(std:("/api/messaging/ingest-external"), params);
}

std::shared_ptr<Promise<std::shared_ptr<MessageChannel>>> MessagingService::createChannel(std::shared_ptr<ChannelCreateParams> params)
{
    auto payload = object{
        object::pair{std:("name"), params->name}, 
        object::pair{std:("type"), params->type}, 
        object::pair{std:("server_id"), OR((params->serverId), ((as<std::shared_ptr<UUID>>(std:("00000000-0000-0000-0000-000000000000")))))}, 
        object::pair{std:("metadata"), params->metadata}
    };
    return this->post<std::shared_ptr<MessageChannel>>(std:("/api/messaging/central-channels"), payload);
}

std::shared_ptr<Promise<std::shared_ptr<MessageChannel>>> MessagingService::createGroupChannel(std::shared_ptr<GroupChannelCreateParams> params)
{
    auto DEFAULT_SERVER_ID = as<std::shared_ptr<UUID>>(std:("00000000-0000-0000-0000-000000000000"));
    any cleanedMetadata;
    any serverIdFromMeta;
    any typeFromMeta;
    if (params->metadata) {
        auto metadataCopy = utils::assign(object{
        }, params->metadata);
        if (in(std:("server_id"), metadataCopy)) {
            serverIdFromMeta = as<any>(const_(metadataCopy)[std:("server_id")]);
            metadataCopy.Delete(std:("server_id"));
        }
        if (in(std:("type"), metadataCopy)) {
            typeFromMeta = as<any>(const_(metadataCopy)[std:("type")]);
            metadataCopy.Delete(std:("type"));
        }
        if (Object->keys(metadataCopy)->get_length() > 0) {
            cleanedMetadata = metadataCopy;
        }
    }
    auto payload = utils::assign(object{
        object::pair{std:("name"), params->name}, 
        object::pair{std:("server_id"), OR((serverIdFromMeta), (DEFAULT_SERVER_ID))}, 
        object::pair{std:("participantCentralUserIds"), params->participantIds}
    }, ((typeFromMeta) ? object{
        object::pair{std:("type"), typeFromMeta}
    } : object{}), ((cleanedMetadata) ? object{
        object::pair{std:("metadata"), cleanedMetadata}
    } : object{}));
    return this->post<std::shared_ptr<MessageChannel>>(std:("/api/messaging/central-channels"), payload);
}

std::shared_ptr<Promise<std::shared_ptr<MessageChannel>>> MessagingService::getOrCreateDmChannel(std::shared_ptr<DmChannelParams> params)
{
    auto [userA, userB] = params->participantIds;
    auto query = object{
        object::pair{std:("currentUserId"), userA}, 
        object::pair{std:("targetUserId"), userB}, 
        object::pair{std:("dmServerId"), as<std::shared_ptr<UUID>>(std:("00000000-0000-0000-0000-000000000000"))}
    };
    return this->get<std::shared_ptr<MessageChannel>>(std:("/api/messaging/dm-channel"), object{
        object::pair{std:("params"), query}
    });
}

std::shared_ptr<Promise<std::shared_ptr<MessageChannel>>> MessagingService::getChannelDetails(std::shared_ptr<UUID> channelId)
{
    return this->get<std::shared_ptr<MessageChannel>>(std:("/api/messaging/central-channels/") + channelId + std:("/details"));
}

std::shared_ptr<Promise<object>> MessagingService::getChannelParticipants(std::shared_ptr<UUID> channelId)
{
    return this->get<object>(std:("/api/messaging/central-channels/") + channelId + std:("/participants"));
}

std::shared_ptr<Promise<object>> MessagingService::addAgentToChannel(std::shared_ptr<UUID> channelId, std::shared_ptr<UUID> agentId)
{
    return this->post<object>(std:("/api/messaging/central-channels/") + channelId + std:("/agents"), object{
        object::pair{std:("agentId"), std:("agentId")}
    });
}

std::shared_ptr<Promise<object>> MessagingService::removeAgentFromChannel(std::shared_ptr<UUID> channelId, std::shared_ptr<UUID> agentId)
{
    return this->delete<object>(std:("/api/messaging/central-channels/") + channelId + std:("/agents/") + agentId + string_empty);
}

std::shared_ptr<Promise<object>> MessagingService::deleteChannel(std::shared_ptr<UUID> channelId)
{
    return this->delete<object>(std:("/api/messaging/central-channels/") + channelId + string_empty);
}

std::shared_ptr<Promise<object>> MessagingService::clearChannelHistory(std::shared_ptr<UUID> channelId)
{
    return this->delete<object>(std:("/api/messaging/central-channels/") + channelId + std:("/messages"));
}

std::shared_ptr<Promise<object>> MessagingService::addAgentToServer(std::shared_ptr<UUID> serverId, std::shared_ptr<UUID> agentId)
{
    return this->post<object>(std:("/api/messaging/servers/") + serverId + std:("/agents"), object{
        object::pair{std:("agentId"), std:("agentId")}
    });
}

std::shared_ptr<Promise<object>> MessagingService::removeAgentFromServer(std::shared_ptr<UUID> serverId, std::shared_ptr<UUID> agentId)
{
    return this->delete<object>(std:("/api/messaging/servers/") + serverId + std:("/agents/") + agentId + string_empty);
}

std::shared_ptr<Promise<std::shared_ptr<Message>>> MessagingService::postMessage(std::shared_ptr<UUID> channelId, string content, std::shared_ptr<MessageMetadata> metadata)
{
    return this->post<std::shared_ptr<Message>>(std:("/api/messaging/central-channels/") + channelId + std:("/messages"), object{
        object::pair{std:("content"), std:("content")}, 
        object::pair{std:("metadata"), std:("metadata")}
    });
}

std::shared_ptr<Promise<object>> MessagingService::getChannelMessages(std::shared_ptr<UUID> channelId, any params)
{
    return this->get<object>(std:("/api/messaging/central-channels/") + channelId + std:("/messages"), object{
        object::pair{std:("params"), std:("params")}
    });
}

std::shared_ptr<Promise<std::shared_ptr<Message>>> MessagingService::getMessage(std::shared_ptr<UUID> messageId)
{
    return this->get<std::shared_ptr<Message>>(std:("/api/messaging/messages/") + messageId + string_empty);
}

std::shared_ptr<Promise<object>> MessagingService::deleteMessage(std::shared_ptr<UUID> channelId, std::shared_ptr<UUID> messageId)
{
    return this->delete<object>(std:("/api/messaging/central-channels/") + channelId + std:("/messages/") + messageId + string_empty);
}

std::shared_ptr<Promise<std::shared_ptr<Message>>> MessagingService::updateMessage(std::shared_ptr<UUID> messageId, string content)
{
    return this->patch<std::shared_ptr<Message>>(std:("/api/messaging/messages/") + messageId + string_empty, object{
        object::pair{std:("content"), std:("content")}
    });
}

std::shared_ptr<Promise<object>> MessagingService::searchMessages(std::shared_ptr<MessageSearchParams> params)
{
    return this->post<object>(std:("/api/messaging/messages/search"), params);
}

std::shared_ptr<Promise<object>> MessagingService::listServers()
{
    return this->get<object>(std:("/api/messaging/central-servers"));
}

std::shared_ptr<Promise<object>> MessagingService::getServerChannels(std::shared_ptr<UUID> serverId)
{
    return this->get<object>(std:("/api/messaging/central-servers/") + serverId + std:("/channels"));
}

std::shared_ptr<Promise<std::shared_ptr<MessageServer>>> MessagingService::createServer(std::shared_ptr<ServerCreateParams> params)
{
    return this->post<std::shared_ptr<MessageServer>>(std:("/api/messaging/servers"), params);
}

std::shared_ptr<Promise<object>> MessagingService::syncServerChannels(std::shared_ptr<UUID> serverId, std::shared_ptr<ServerSyncParams> params)
{
    return this->post<object>(std:("/api/messaging/servers/") + serverId + std:("/sync-channels"), params);
}

std::shared_ptr<Promise<object>> MessagingService::deleteServer(std::shared_ptr<UUID> serverId)
{
    return this->delete<object>(std:("/api/messaging/servers/") + serverId + string_empty);
}

std::shared_ptr<Promise<object>> MessagingService::updateChannel(std::shared_ptr<UUID> channelId, std::shared_ptr<ChannelUpdateParams> params)
{
    return this->patch<object>(std:("/api/messaging/central-channels/") + channelId + string_empty, params);
}

std::shared_ptr<Promise<object>> MessagingService::generateChannelTitle(string userMessage, std::shared_ptr<UUID> agentId)
{
    return this->post<object>(std:("/api/messaging/generate-title"), object{
        object::pair{std:("userMessage"), std:("userMessage")}, 
        object::pair{std:("agentId"), std:("agentId")}
    });
}

std::shared_ptr<Promise<object>> MessagingService::generateChannelPrompts(std::shared_ptr<UUID> channelId, std::shared_ptr<UUID> agentId, double count)
{
    return this->post<object>(std:("/api/messaging/central-channels/") + channelId + std:("/generate-prompts"), object{
        object::pair{std:("agentId"), std:("agentId")}, 
        object::pair{std:("count"), std:("count")}
    });
}

std::shared_ptr<Promise<object>> MessagingService::addUserToChannel(std::shared_ptr<UUID> channelId, std::shared_ptr<UUID> userId)
{
    auto channel = std::async([=]() { this->getChannelDetails(channelId); });
    auto currentParticipants = OR((channel->metadata->participantCentralUserIds), (array<any>()));
    if (!currentParticipants->includes(userId)) {
        auto updatedParticipants = array<std::shared_ptr<UUID>>{ currentParticipants, userId };
        return this->updateChannel(channelId, object{
            object::pair{std:("participantCentralUserIds"), updatedParticipants}
        });
    }
    return object{
        object::pair{std:("success"), true}, 
        object::pair{std:("data"), channel}
    };
}

std::shared_ptr<Promise<object>> MessagingService::addUsersToChannel(std::shared_ptr<UUID> channelId, array<std::shared_ptr<UUID>> userIds)
{
    auto channel = std::async([=]() { this->getChannelDetails(channelId); });
    auto currentParticipants = OR((channel->metadata->participantCentralUserIds), (array<any>()));
    auto newParticipants = array<std::shared_ptr<UUID>>{ currentParticipants };
    for (auto& userId : userIds)
    {
        if (!newParticipants->includes(userId)) {
            newParticipants->push(userId);
        }
    }
    return this->updateChannel(channelId, object{
        object::pair{std:("participantCentralUserIds"), newParticipants}
    });
}

std::shared_ptr<Promise<object>> MessagingService::removeUserFromChannel(std::shared_ptr<UUID> channelId, std::shared_ptr<UUID> userId)
{
    auto channel = std::async([=]() { this->getChannelDetails(channelId); });
    auto currentParticipants = OR((channel->metadata->participantCentralUserIds), (array<any>()));
    auto updatedParticipants = currentParticipants->filter([=](auto id) mutable
    {
        return id != userId;
    }
    );
    return this->updateChannel(channelId, object{
        object::pair{std:("participantCentralUserIds"), updatedParticipants}
    });
}

std::shared_ptr<Promise<std::shared_ptr<CreateJobResponse>>> MessagingService::createJob(std::shared_ptr<CreateJobRequest> params)
{
    return this->post<std::shared_ptr<CreateJobResponse>>(std:("/api/messaging/jobs"), params);
}

std::shared_ptr<Promise<std::shared_ptr<JobDetailsResponse>>> MessagingService::getJob(string jobId)
{
    return this->get<std::shared_ptr<JobDetailsResponse>>(std:("/api/messaging/jobs/") + jobId + string_empty);
}

std::shared_ptr<Promise<std::shared_ptr<JobListResponse>>> MessagingService::listJobs(std::shared_ptr<ListJobsParams> params)
{
    return this->get<std::shared_ptr<JobListResponse>>(std:("/api/messaging/jobs"), object{
        object::pair{std:("params"), std:("params")}
    });
}

std::shared_ptr<Promise<std::shared_ptr<JobHealthResponse>>> MessagingService::getJobsHealth()
{
    return this->get<std::shared_ptr<JobHealthResponse>>(std:("/api/messaging/jobs/health"));
}

std::shared_ptr<Promise<std::shared_ptr<JobDetailsResponse>>> MessagingService::pollJob(string jobId, double interval, double maxAttempts)
{
    auto attempts = 0;
    while (attempts < maxAttempts)
    {
        auto job = std::async([=]() { this->getJob(jobId); });
        if (static_cast<long>(job->status) == JobStatus::COMPLETED) {
            return job;
        }
        if (job->status == JobStatus::FAILED) {
            throw any(std::make_shared<Error>(std:("Job failed: ") + (OR((job->error), (std:("Unknown error")))) + string_empty));
        }
        if (job->status == JobStatus::TIMEOUT) {
            throw any(std::make_shared<Error>(std:("Job timed out waiting for agent response")));
        }
        std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
        {
            return setTimeout(resolve, interval);
        }
        ); });
        attempts++;
    }
    throw any(std::make_shared<Error>(std:("Polling exceeded maximum attempts (") + maxAttempts + std:(")")));
}

std::shared_ptr<Promise<std::shared_ptr<JobDetailsResponse>>> MessagingService::createAndWaitForJob(std::shared_ptr<CreateJobRequest> params, double pollInterval, double maxAttempts)
{
    auto job = std::async([=]() { this->createJob(params); });
    return this->pollJob(job->jobId, pollInterval, maxAttempts);
}

MessagingService::MessagingService(std::shared_ptr<ApiClientConfig> config) : BaseApiClient(config) {
}

