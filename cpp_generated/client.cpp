#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/api-client/src/client.h"

ElizaClient::ElizaClient(std::shared_ptr<ApiClientConfig> config) {
    this->agents = std::make_shared<AgentsService>(config);
    this->messaging = std::make_shared<MessagingService>(config);
    this->memory = std::make_shared<MemoryService>(config);
    this->audio = std::make_shared<AudioService>(config);
    this->media = std::make_shared<MediaService>(config);
    this->server = std::make_shared<ServerService>(config);
    this->system = std::make_shared<SystemService>(config);
}

std::shared_ptr<ElizaClient> ElizaClient::create(std::shared_ptr<ApiClientConfig> config)
{
    return std::make_shared<ElizaClient>(config);
}

