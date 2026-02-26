#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_OTAKU_SRC_PACKAGES_API-CLIENT_SRC_TYPES_ENTITIES_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_OTAKU_SRC_PACKAGES_API-CLIENT_SRC_TYPES_ENTITIES_H
#include "core.h"
#include "@elizaos/core.h"

class Entity;
class EntityCreateParams;
class EntityUpdateParams;

class Entity : public object, public std::enable_shared_from_this<Entity> {
public:
    using std::enable_shared_from_this<Entity>::shared_from_this;
    std::shared_ptr<UUID> id;

    std::shared_ptr<UUID> agentId;

    array<string> names;

    Record<string, any> metadata;
};

class EntityCreateParams : public object, public std::enable_shared_from_this<EntityCreateParams> {
public:
    using std::enable_shared_from_this<EntityCreateParams>::shared_from_this;
    std::shared_ptr<UUID> id;

    std::shared_ptr<UUID> agentId;

    array<string> names;

    Record<string, any> metadata;
};

class EntityUpdateParams : public object, public std::enable_shared_from_this<EntityUpdateParams> {
public:
    using std::enable_shared_from_this<EntityUpdateParams>::shared_from_this;
    array<string> names;

    Record<string, any> metadata;

    std::shared_ptr<UUID> agentId;
};

#endif
