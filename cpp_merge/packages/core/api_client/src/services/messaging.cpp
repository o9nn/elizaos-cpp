#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/api-client/src/services/messaging.h"

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
    return this->post<std::shared_ptr<MessageChannel>>(std:("/api/messaging/central-channels"), params);
}

std::shared_ptr<Promise<std::shared_ptr<MessageChannel>>> MessagingService::createGroupChannel(std::shared_ptr<GroupChannelCreateParams> params)
{
    return this->post<std::shared_ptr<MessageChannel>>(std:("/api/messaging/central-channels"), params);
}

std::shared_ptr<Promise<std::shared_ptr<MessageChannel>>> MessagingService::getOrCreateDmChannel(std::shared_ptr<DmChannelParams> params)
{
    return this->get<std::shared_ptr<MessageChannel>>(std:("/api/messaging/dm-channel"), object{
        object::pair{std:("params"), std:("params")}
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

std::shared_ptr<Promise<std::shared_ptr<Message>>> MessagingService::postMessage(std::shared_ptr<UUID> channelId, string content, Record<string, any> metadata)
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

std::shared_ptr<Promise<object>> MessagingService::generateChannelTitle(std::shared_ptr<UUID> channelId, std::shared_ptr<UUID> agentId)
{
    return this->post<object>(std:("/api/messaging/central-channels/") + channelId + std:("/generate-title"), object{
        object::pair{std:("agentId"), std:("agentId")}
    });
}

std::shared_ptr<Promise<object>> MessagingService::addUserToChannel(std::shared_ptr<UUID> channelId, std::shared_ptr<UUID> userId)
{
    auto channel = std::async([=]() { this->getChannelDetails(channelId); });
    auto currentParticipants = OR((channel->metadata->participantCentralUserIds), (array<any>()));
    if (!currentParticipants["includes"](userId)) {
        auto updatedParticipants = array<any>{ currentParticipants, userId };
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
    auto newParticipants = array<any>{ currentParticipants };
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
    auto updatedParticipants = currentParticipants["filter"]([=](auto id) mutable
    {
        return id != userId;
    }
    );
    return this->updateChannel(channelId, object{
        object::pair{std:("participantCentralUserIds"), updatedParticipants}
    });
}

MessagingService::MessagingService(std::shared_ptr<ApiClientConfig> config) : BaseApiClient(config) {
}

