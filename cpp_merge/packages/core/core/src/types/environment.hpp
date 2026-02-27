#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CORE_SRC_TYPES_ENVIRONMENT_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CORE_SRC_TYPES_ENVIRONMENT_H
#include "core.h"
#include "./primitives.h"

typedef object World;

typedef object Room;

typedef object RoomMetadata;

class Component;
class Entity;
enum struct Role;
enum struct ChannelType;
class Participant;
class Relationship;

class Component : public object, public std::enable_shared_from_this<Component> {
public:
    using std::enable_shared_from_this<Component>::shared_from_this;
    std::shared_ptr<UUID> id;

    std::shared_ptr<UUID> entityId;

    std::shared_ptr<UUID> agentId;

    std::shared_ptr<UUID> roomId;

    std::shared_ptr<UUID> worldId;

    std::shared_ptr<UUID> sourceEntityId;

    string type;

    double createdAt;

    Metadata data;
};

class Entity : public object, public std::enable_shared_from_this<Entity> {
public:
    using std::enable_shared_from_this<Entity>::shared_from_this;
    std::shared_ptr<UUID> id;

    array<string> names;

    Metadata metadata;

    std::shared_ptr<UUID> agentId;

    array<std::shared_ptr<Component>> components;
};

enum struct Role {
    OWNER = std:("OWNER"), ADMIN = std:("ADMIN"), NONE = std:("NONE")
};
enum struct ChannelType {
    SELF = std:("SELF"), DM = std:("DM"), GROUP = std:("GROUP"), VOICE_DM = std:("VOICE_DM"), VOICE_GROUP = std:("VOICE_GROUP"), FEED = std:("FEED"), THREAD = std:("THREAD"), WORLD = std:("WORLD"), FORUM = std:("FORUM"), API = std:("API")
};
class Participant : public object, public std::enable_shared_from_this<Participant> {
public:
    using std::enable_shared_from_this<Participant>::shared_from_this;
    std::shared_ptr<UUID> id;

    std::shared_ptr<Entity> entity;
};

class Relationship : public object, public std::enable_shared_from_this<Relationship> {
public:
    using std::enable_shared_from_this<Relationship>::shared_from_this;
    std::shared_ptr<UUID> id;

    std::shared_ptr<UUID> sourceEntityId;

    std::shared_ptr<UUID> targetEntityId;

    std::shared_ptr<UUID> agentId;

    array<string> tags;

    Metadata metadata;

    string createdAt;
};

#endif
