#include "media.hpp"

std::shared_ptr<Promise<std::shared_ptr<MediaUploadResponse>>> MediaService::uploadAgentMedia(std::shared_ptr<UUID> agentId, std::shared_ptr<MediaUploadParams> params)
{
    auto formData = std::make_shared<FormData>();
    formData->append(std:("file"), params->file, params->filename);
    if (params->contentType) formData->append(std:("contentType"), params->contentType);
    if (params->metadata) formData->append(std:("metadata"), JSON->stringify(params->metadata));
    return this->request<std::shared_ptr<MediaUploadResponse>>(std:("POST"), std:("/api/media/agents/") + agentId + std:("/upload-media"), object{
        object::pair{std:("body"), formData}
    });
}

std::shared_ptr<Promise<std::shared_ptr<ChannelUploadResponse>>> MediaService::uploadChannelMedia(std::shared_ptr<UUID> channelId, std::shared_ptr<File> file)
{
    auto formData = std::make_shared<FormData>();
    formData->append(std:("file"), file);
    return this->request<std::shared_ptr<ChannelUploadResponse>>(std:("POST"), std:("/api/messaging/central-channels/") + channelId + std:("/upload-media"), object{
        object::pair{std:("body"), formData}
    });
}

MediaService::MediaService(std::shared_ptr<ApiClientConfig> config) : BaseApiClient(config) {
}

