#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/api-client/src/services/media.h"

std::shared_ptr<Promise<std::shared_ptr<MediaUploadResponse>>> MediaService::uploadAgentMedia(std::shared_ptr<UUID> agentId, std::shared_ptr<MediaUploadParams> params)
{
    auto formData = std::make_shared<FormData>();
    formData->append(std::string("file"), params->file, params->filename);
    if (params->contentType) formData->append(std::string("contentType"), params->contentType);
    if (params->metadata) formData->append(std::string("metadata"), JSON->stringify(params->metadata));
    return this->request<std::shared_ptr<MediaUploadResponse>>(std::string("POST"), std::string("/api/media/agents/") + agentId + std::string("/upload-media"), object{
        object::pair{std::string("body"), formData}
    });
}

std::shared_ptr<Promise<std::shared_ptr<ChannelUploadResponse>>> MediaService::uploadChannelMedia(std::shared_ptr<UUID> channelId, std::shared_ptr<File> file)
{
    auto formData = std::make_shared<FormData>();
    formData->append(std::string("file"), file);
    return this->request<std::shared_ptr<ChannelUploadResponse>>(std::string("POST"), std::string("/api/messaging/central-channels/") + channelId + std::string("/upload-media"), object{
        object::pair{std::string("body"), formData}
    });
}

MediaService::MediaService(std::shared_ptr<ApiClientConfig> config) : BaseApiClient(config) {
}

