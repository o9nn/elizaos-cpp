#include "/home/runner/work/elizaos-cpp/elizaos-cpp/otaku/src/packages/api-client/src/services/messaging.h"

std::shared_ptr<Promise<std::shared_ptr<Message>>> MessagingService::submitMessage(std::shared_ptr<MessageSubmitParams> params)
{
    return this->post<std::shared_ptr<Message>>(std::string("/api/messaging/submit"), params);
}

std::shared_ptr<Promise<object>> MessagingService::completeMessage(std::shared_ptr<MessageCompleteParams> params)
{
    return this->post<object>(std::string("/api/messaging/complete"), params);
}

std::shared_ptr<Promise<object>> MessagingService::ingestExternalMessages(std::shared_ptr<ExternalMessageParams> params)
{
    return this->post<object>(std::string("/api/messaging/ingest-external"), params);
}

std::shared_ptr<Promise<std::shared_ptr<MessageChannel>>> MessagingService::createChannel(std::shared_ptr<ChannelCreateParams> params)
{
    auto payload = object{
        object::pair{std::string("name"), params->name}, 
        object::pair{std::string("type"), params->type}, 
        object::pair{std::string("server_id"), OR((params->serverId), ((as<std::shared_ptr<UUID>>(std::string("00000000-0000-0000-0000-000000000000")))))}, 
        object::pair{std::string("metadata"), params->metadata}
    };
    return this->post<std::shared_ptr<MessageChannel>>(std::string("/api/messaging/central-channels"), payload);
}

std::shared_ptr<Promise<std::shared_ptr<MessageChannel>>> MessagingService::createGroupChannel(std::shared_ptr<GroupChannelCreateParams> params)
{
    auto DEFAULT_SERVER_ID = as<std::shared_ptr<UUID>>(std::string("00000000-0000-0000-0000-000000000000"));
    any cleanedMetadata;
    any serverIdFromMeta;
    any typeFromMeta;
    if (params->metadata) {
        auto metadataCopy = utils::assign(object{
        }, params->metadata);
        if (in(std::string("server_id"), metadataCopy)) {
            serverIdFromMeta = as<any>(const_(metadataCopy)[std::string("server_id")]);
            metadataCopy.Delete(std::string("server_id"));
        }
        if (in(std::string("type"), metadataCopy)) {
            typeFromMeta = as<any>(const_(metadataCopy)[std::string("type")]);
            metadataCopy.Delete(std::string("type"));
        }
        if (Object->keys(metadataCopy)->get_length() > 0) {
            cleanedMetadata = metadataCopy;
        }
    }
    auto payload = utils::assign(object{
        object::pair{std::string("name"), params->name}, 
        object::pair{std::string("server_id"), OR((serverIdFromMeta), (DEFAULT_SERVER_ID))}, 
        object::pair{std::string("participantCentralUserIds"), params->participantIds}
    }, ((typeFromMeta) ? object{
        object::pair{std::string("type"), typeFromMeta}
    } : object{}), ((cleanedMetadata) ? object{
        object::pair{std::string("metadata"), cleanedMetadata}
    } : object{}));
    return this->post<std::shared_ptr<MessageChannel>>(std::string("/api/messaging/central-channels"), payload);
}

std::shared_ptr<Promise<std::shared_ptr<MessageChannel>>> MessagingService::getOrCreateDmChannel(std::shared_ptr<DmChannelParams> params)
{
    auto [userA, userB] = params->participantIds;
    auto query = object{
        object::pair{std::string("currentUserId"), userA}, 
        object::pair{std::string("targetUserId"), userB}, 
        object::pair{std::string("dmServerId"), as<std::shared_ptr<UUID>>(std::string("00000000-0000-0000-0000-000000000000"))}
    };
    return this->get<std::shared_ptr<MessageChannel>>(std::string("/api/messaging/dm-channel"), object{
        object::pair{std::string("params"), query}
    });
}

std::shared_ptr<Promise<std::shared_ptr<MessageChannel>>> MessagingService::getChannelDetails(std::shared_ptr<UUID> channelId)
{
    return this->get<std::shared_ptr<MessageChannel>>(std::string("/api/messaging/central-channels/") + channelId + std::string("/details"));
}

std::shared_ptr<Promise<object>> MessagingService::getChannelParticipants(std::shared_ptr<UUID> channelId)
{
    return this->get<object>(std::string("/api/messaging/central-channels/") + channelId + std::string("/participants"));
}

std::shared_ptr<Promise<object>> MessagingService::addAgentToChannel(std::shared_ptr<UUID> channelId, std::shared_ptr<UUID> agentId)
{
    return this->post<object>(std::string("/api/messaging/central-channels/") + channelId + std::string("/agents"), object{
        object::pair{std::string("agentId"), std::string("agentId")}
    });
}

std::shared_ptr<Promise<object>> MessagingService::removeAgentFromChannel(std::shared_ptr<UUID> channelId, std::shared_ptr<UUID> agentId)
{
    return this->delete<object>(std::string("/api/messaging/central-channels/") + channelId + std::string("/agents/") + agentId + string_empty);
}

std::shared_ptr<Promise<object>> MessagingService::deleteChannel(std::shared_ptr<UUID> channelId)
{
    return this->delete<object>(std::string("/api/messaging/central-channels/") + channelId + string_empty);
}

std::shared_ptr<Promise<object>> MessagingService::clearChannelHistory(std::shared_ptr<UUID> channelId)
{
    return this->delete<object>(std::string("/api/messaging/central-channels/") + channelId + std::string("/messages"));
}

std::shared_ptr<Promise<object>> MessagingService::addAgentToServer(std::shared_ptr<UUID> serverId, std::shared_ptr<UUID> agentId)
{
    return this->post<object>(std::string("/api/messaging/servers/") + serverId + std::string("/agents"), object{
        object::pair{std::string("agentId"), std::string("agentId")}
    });
}

std::shared_ptr<Promise<object>> MessagingService::removeAgentFromServer(std::shared_ptr<UUID> serverId, std::shared_ptr<UUID> agentId)
{
    return this->delete<object>(std::string("/api/messaging/servers/") + serverId + std::string("/agents/") + agentId + string_empty);
}

std::shared_ptr<Promise<std::shared_ptr<Message>>> MessagingService::postMessage(std::shared_ptr<UUID> channelId, string content, std::shared_ptr<MessageMetadata> metadata)
{
    return this->post<std::shared_ptr<Message>>(std::string("/api/messaging/central-channels/") + channelId + std::string("/messages"), object{
        object::pair{std::string("content"), std::string("content")}, 
        object::pair{std::string("metadata"), std::string("metadata")}
    });
}

std::shared_ptr<Promise<object>> MessagingService::getChannelMessages(std::shared_ptr<UUID> channelId, any params)
{
    return this->get<object>(std::string("/api/messaging/central-channels/") + channelId + std::string("/messages"), object{
        object::pair{std::string("params"), std::string("params")}
    });
}

std::shared_ptr<Promise<std::shared_ptr<Message>>> MessagingService::getMessage(std::shared_ptr<UUID> messageId)
{
    return this->get<std::shared_ptr<Message>>(std::string("/api/messaging/messages/") + messageId + string_empty);
}

std::shared_ptr<Promise<object>> MessagingService::deleteMessage(std::shared_ptr<UUID> channelId, std::shared_ptr<UUID> messageId)
{
    return this->delete<object>(std::string("/api/messaging/central-channels/") + channelId + std::string("/messages/") + messageId + string_empty);
}

std::shared_ptr<Promise<std::shared_ptr<Message>>> MessagingService::updateMessage(std::shared_ptr<UUID> messageId, string content)
{
    return this->patch<std::shared_ptr<Message>>(std::string("/api/messaging/messages/") + messageId + string_empty, object{
        object::pair{std::string("content"), std::string("content")}
    });
}

std::shared_ptr<Promise<object>> MessagingService::searchMessages(std::shared_ptr<MessageSearchParams> params)
{
    return this->post<object>(std::string("/api/messaging/messages/search"), params);
}

std::shared_ptr<Promise<object>> MessagingService::listServers()
{
    return this->get<object>(std::string("/api/messaging/central-servers"));
}

std::shared_ptr<Promise<object>> MessagingService::getServerChannels(std::shared_ptr<UUID> serverId)
{
    return this->get<object>(std::string("/api/messaging/central-servers/") + serverId + std::string("/channels"));
}

std::shared_ptr<Promise<std::shared_ptr<MessageServer>>> MessagingService::createServer(std::shared_ptr<ServerCreateParams> params)
{
    return this->post<std::shared_ptr<MessageServer>>(std::string("/api/messaging/servers"), params);
}

std::shared_ptr<Promise<object>> MessagingService::syncServerChannels(std::shared_ptr<UUID> serverId, std::shared_ptr<ServerSyncParams> params)
{
    return this->post<object>(std::string("/api/messaging/servers/") + serverId + std::string("/sync-channels"), params);
}

std::shared_ptr<Promise<object>> MessagingService::deleteServer(std::shared_ptr<UUID> serverId)
{
    return this->delete<object>(std::string("/api/messaging/servers/") + serverId + string_empty);
}

std::shared_ptr<Promise<object>> MessagingService::updateChannel(std::shared_ptr<UUID> channelId, std::shared_ptr<ChannelUpdateParams> params)
{
    return this->patch<object>(std::string("/api/messaging/central-channels/") + channelId + string_empty, params);
}

std::shared_ptr<Promise<object>> MessagingService::generateChannelTitle(string userMessage, std::shared_ptr<UUID> agentId)
{
    return this->post<object>(std::string("/api/messaging/generate-title"), object{
        object::pair{std::string("userMessage"), std::string("userMessage")}, 
        object::pair{std::string("agentId"), std::string("agentId")}
    });
}

std::shared_ptr<Promise<object>> MessagingService::generateChannelPrompts(std::shared_ptr<UUID> channelId, std::shared_ptr<UUID> agentId, double count)
{
    return this->post<object>(std::string("/api/messaging/central-channels/") + channelId + std::string("/generate-prompts"), object{
        object::pair{std::string("agentId"), std::string("agentId")}, 
        object::pair{std::string("count"), std::string("count")}
    });
}

std::shared_ptr<Promise<object>> MessagingService::addUserToChannel(std::shared_ptr<UUID> channelId, std::shared_ptr<UUID> userId)
{
    auto channel = std::async([=]() { this->getChannelDetails(channelId); });
    auto currentParticipants = OR((channel->metadata->participantCentralUserIds), (array<any>()));
    if (!currentParticipants->includes(userId)) {
        auto updatedParticipants = array<std::shared_ptr<UUID>>{ currentParticipants, userId };
        return this->updateChannel(channelId, object{
            object::pair{std::string("participantCentralUserIds"), updatedParticipants}
        });
    }
    return object{
        object::pair{std::string("success"), true}, 
        object::pair{std::string("data"), channel}
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
        object::pair{std::string("participantCentralUserIds"), newParticipants}
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
        object::pair{std::string("participantCentralUserIds"), updatedParticipants}
    });
}

std::shared_ptr<Promise<std::shared_ptr<CreateJobResponse>>> MessagingService::createJob(std::shared_ptr<CreateJobRequest> params)
{
    return this->post<std::shared_ptr<CreateJobResponse>>(std::string("/api/messaging/jobs"), params);
}

std::shared_ptr<Promise<std::shared_ptr<JobDetailsResponse>>> MessagingService::getJob(string jobId)
{
    return this->get<std::shared_ptr<JobDetailsResponse>>(std::string("/api/messaging/jobs/") + jobId + string_empty);
}

std::shared_ptr<Promise<std::shared_ptr<JobListResponse>>> MessagingService::listJobs(std::shared_ptr<ListJobsParams> params)
{
    return this->get<std::shared_ptr<JobListResponse>>(std::string("/api/messaging/jobs"), object{
        object::pair{std::string("params"), std::string("params")}
    });
}

std::shared_ptr<Promise<std::shared_ptr<JobHealthResponse>>> MessagingService::getJobsHealth()
{
    return this->get<std::shared_ptr<JobHealthResponse>>(std::string("/api/messaging/jobs/health"));
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
            throw any(std::make_shared<Error>(std::string("Job failed: ") + (OR((job->error), (std::string("Unknown error")))) + string_empty));
        }
        if (job->status == JobStatus::TIMEOUT) {
            throw any(std::make_shared<Error>(std::string("Job timed out waiting for agent response")));
        }
        std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
        {
            return setTimeout(resolve, interval);
        }
        ); });
        attempts++;
    }
    throw any(std::make_shared<Error>(std::string("Polling exceeded maximum attempts (") + maxAttempts + std::string(")")));
}

std::shared_ptr<Promise<std::shared_ptr<JobDetailsResponse>>> MessagingService::createAndWaitForJob(std::shared_ptr<CreateJobRequest> params, double pollInterval, double maxAttempts)
{
    auto job = std::async([=]() { this->createJob(params); });
    return this->pollJob(job->jobId, pollInterval, maxAttempts);
}

MessagingService::MessagingService(std::shared_ptr<ApiClientConfig> config) : BaseApiClient(config) {
}

