#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_OTAKU_SRC_PACKAGES_API_CLIENT_SRC_TYPES_MEMORY_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_OTAKU_SRC_PACKAGES_API_CLIENT_SRC_TYPES_MEMORY_H
#include "core.hpp"
// External dependency removed
#include "./base.h"

class Memory;
class Room;
class MemoryParams;
class MemoryUpdateParams;
class RoomCreateParams;
class WorldCreateParams;

class Memory : public object, public std::enable_shared_from_this<Memory> {
public:
    using std::enable_shared_from_this<Memory>::shared_from_this;
    std::shared_ptr<UUID> id;

    std::shared_ptr<UUID> agentId;

    std::shared_ptr<UUID> roomId;

    string type;

    any content;

    array<double> embedding;

    std::shared_ptr<Date> createdAt;

    std::shared_ptr<Date> updatedAt;

    Record<string, any> metadata;
};

class Room : public object, public std::enable_shared_from_this<Room> {
public:
    using std::enable_shared_from_this<Room>::shared_from_this;
    std::shared_ptr<UUID> id;

    std::shared_ptr<UUID> agentId;

    string name;

    string type;

    std::shared_ptr<Date> createdAt;

    std::shared_ptr<Date> updatedAt;

    Record<string, any> metadata;
};

class MemoryParams : public PaginationParams, public std::enable_shared_from_this<MemoryParams> {
public:
    using std::enable_shared_from_this<MemoryParams>::shared_from_this;
    string type;

    string search;

    any from;

    any to;
};

class MemoryUpdateParams : public object, public std::enable_shared_from_this<MemoryUpdateParams> {
public:
    using std::enable_shared_from_this<MemoryUpdateParams>::shared_from_this;
    any content;

    Record<string, any> metadata;
};

class RoomCreateParams : public object, public std::enable_shared_from_this<RoomCreateParams> {
public:
    using std::enable_shared_from_this<RoomCreateParams>::shared_from_this;
    string name;

    string type;

    Record<string, any> metadata;
};

class WorldCreateParams : public object, public std::enable_shared_from_this<WorldCreateParams> {
public:
    using std::enable_shared_from_this<WorldCreateParams>::shared_from_this;
    std::shared_ptr<UUID> serverId;

    string name;

    string description;
};

#endif
