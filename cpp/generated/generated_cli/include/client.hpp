#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_API-CLIENT_SRC_CLIENT_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_API-CLIENT_SRC_CLIENT_H
#include "core.h"
#include "./types/base.h"
#include "./services/agents.h"
#include "./services/messaging.h"
#include "./services/memory.h"
#include "./services/audio.h"
#include "./services/media.h"
#include "./services/server.h"
#include "./services/system.h"

class ElizaClient;

class ElizaClient : public object, public std::enable_shared_from_this<ElizaClient> {
public:
    using std::enable_shared_from_this<ElizaClient>::shared_from_this;
    std::shared_ptr<AgentsService> agents;

    std::shared_ptr<MessagingService> messaging;

    std::shared_ptr<MemoryService> memory;

    std::shared_ptr<AudioService> audio;

    std::shared_ptr<MediaService> media;

    std::shared_ptr<ServerService> server;

    std::shared_ptr<SystemService> system;

    ElizaClient(std::shared_ptr<ApiClientConfig> config);
    static std::shared_ptr<ElizaClient> create(std::shared_ptr<ApiClientConfig> config);
};

#endif
