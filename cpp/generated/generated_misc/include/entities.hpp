#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_OTAKU_SRC_PACKAGES_API-CLIENT_SRC_SERVICES_ENTITIES_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_OTAKU_SRC_PACKAGES_API-CLIENT_SRC_SERVICES_ENTITIES_H
#include "core.h"
#include "@elizaos/core.h"
#include "../lib/base-client.h"
#include "../types/entities.h"

class EntitiesService;

class EntitiesService : public BaseApiClient, public std::enable_shared_from_this<EntitiesService> {
public:
    using std::enable_shared_from_this<EntitiesService>::shared_from_this;
    virtual std::shared_ptr<Promise<std::shared_ptr<Entity>>> getEntity(std::shared_ptr<UUID> entityId);
    virtual std::shared_ptr<Promise<std::shared_ptr<Entity>>> createEntity(std::shared_ptr<EntityCreateParams> params);
    virtual std::shared_ptr<Promise<std::shared_ptr<Entity>>> updateEntity(std::shared_ptr<UUID> entityId, std::shared_ptr<EntityUpdateParams> params);
    EntitiesService(std::shared_ptr<ApiClientConfig> config);
};

#endif
