#include "/home/runner/work/elizaos-cpp/elizaos-cpp/otaku/src/packages/api-client/src/services/entities.h"

std::shared_ptr<Promise<std::shared_ptr<Entity>>> EntitiesService::getEntity(std::shared_ptr<UUID> entityId)
{
    auto response = std::async([=]() { this->get<object>(std::string("/api/entities/") + entityId + string_empty); });
    return response["entity"];
}

std::shared_ptr<Promise<std::shared_ptr<Entity>>> EntitiesService::createEntity(std::shared_ptr<EntityCreateParams> params)
{
    auto response = std::async([=]() { this->post<object>(std::string("/api/entities"), params); });
    return response["entity"];
}

std::shared_ptr<Promise<std::shared_ptr<Entity>>> EntitiesService::updateEntity(std::shared_ptr<UUID> entityId, std::shared_ptr<EntityUpdateParams> params)
{
    auto response = std::async([=]() { this->patch<object>(std::string("/api/entities/") + entityId + string_empty, params); });
    return response["entity"];
}

EntitiesService::EntitiesService(std::shared_ptr<ApiClientConfig> config) : BaseApiClient(config) {
}

