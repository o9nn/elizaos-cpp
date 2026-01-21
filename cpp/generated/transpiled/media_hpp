#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_API-CLIENT_SRC_SERVICES_MEDIA_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_API-CLIENT_SRC_SERVICES_MEDIA_H
#include "core.h"
#include "@elizaos/core.h"
#include "../lib/base-client.h"
#include "../types/media.h"

class MediaService;

class MediaService : public BaseApiClient, public std::enable_shared_from_this<MediaService> {
public:
    using std::enable_shared_from_this<MediaService>::shared_from_this;
    virtual std::shared_ptr<Promise<std::shared_ptr<MediaUploadResponse>>> uploadAgentMedia(std::shared_ptr<UUID> agentId, std::shared_ptr<MediaUploadParams> params);
    virtual std::shared_ptr<Promise<std::shared_ptr<ChannelUploadResponse>>> uploadChannelMedia(std::shared_ptr<UUID> channelId, std::shared_ptr<File> file);
    MediaService(std::shared_ptr<ApiClientConfig> config);
};

#endif
