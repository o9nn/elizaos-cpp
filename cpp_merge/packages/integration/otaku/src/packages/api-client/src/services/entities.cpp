#include "entities.hpp"

std::shared_ptr<Promise<std::shared_ptr<Entity>>> EntitiesService::getEntity(std::shared_ptr<UUID> entityId)
{
    auto response = std::async([=]() { this->get<object>(std:("/api/entities/") + entityId + string_empty); });
    return response["entity"];
}

std::shared_ptr<Promise<std::shared_ptr<Entity>>> EntitiesService::createEntity(std::shared_ptr<EntityCreateParams> params)
{
    auto response = std::async([=]() { this->post<object>(std:("/api/entities"), params); });
    return response["entity"];
}

std::shared_ptr<Promise<std::shared_ptr<Entity>>> EntitiesService::updateEntity(std::shared_ptr<UUID> entityId, std::shared_ptr<EntityUpdateParams> params)
{
    auto response = std::async([=]() { this->patch<object>(std:("/api/entities/") + entityId + string_empty, params); });
    return response["entity"];
}

EntitiesService::EntitiesService(std::shared_ptr<ApiClientConfig> config) : BaseApiClient(config) {
}

