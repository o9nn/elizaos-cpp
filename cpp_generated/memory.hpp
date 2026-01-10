#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_API-CLIENT_SRC_SERVICES_MEMORY_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_API-CLIENT_SRC_SERVICES_MEMORY_H
#include "core.h"
#include "@elizaos/core.h"
#include "../lib/base-client.h"
#include "../types/memory.h"

class MemoryService;

class MemoryService : public BaseApiClient, public std::enable_shared_from_this<MemoryService> {
public:
    using std::enable_shared_from_this<MemoryService>::shared_from_this;
    virtual std::shared_ptr<Promise<object>> getAgentMemories(std::shared_ptr<UUID> agentId, std::shared_ptr<MemoryParams> params = undefined);
    virtual std::shared_ptr<Promise<object>> getRoomMemories(std::shared_ptr<UUID> agentId, std::shared_ptr<UUID> roomId, std::shared_ptr<MemoryParams> params = undefined);
    virtual std::shared_ptr<Promise<std::shared_ptr<Memory>>> updateMemory(std::shared_ptr<UUID> agentId, std::shared_ptr<UUID> memoryId, std::shared_ptr<MemoryUpdateParams> params);
    virtual std::shared_ptr<Promise<object>> clearAgentMemories(std::shared_ptr<UUID> agentId);
    virtual std::shared_ptr<Promise<object>> clearRoomMemories(std::shared_ptr<UUID> agentId, std::shared_ptr<UUID> roomId);
    virtual std::shared_ptr<Promise<object>> listAgentRooms(std::shared_ptr<UUID> agentId);
    virtual std::shared_ptr<Promise<std::shared_ptr<Room>>> getRoom(std::shared_ptr<UUID> agentId, std::shared_ptr<UUID> roomId);
    virtual std::shared_ptr<Promise<std::shared_ptr<Room>>> createRoom(std::shared_ptr<UUID> agentId, std::shared_ptr<RoomCreateParams> params);
    virtual std::shared_ptr<Promise<object>> createWorldFromServer(std::shared_ptr<UUID> serverId, std::shared_ptr<WorldCreateParams> params);
    virtual std::shared_ptr<Promise<object>> deleteWorld(std::shared_ptr<UUID> serverId);
    virtual std::shared_ptr<Promise<object>> clearWorldMemories(std::shared_ptr<UUID> serverId);
    virtual std::shared_ptr<Promise<object>> deleteMemory(std::shared_ptr<UUID> agentId, std::shared_ptr<UUID> memoryId);
    virtual std::shared_ptr<Promise<object>> getAgentInternalMemories(std::shared_ptr<UUID> agentId, std::shared_ptr<UUID> agentPerspectiveRoomId, boolean includeEmbedding = undefined);
    virtual std::shared_ptr<Promise<object>> deleteAgentInternalMemory(std::shared_ptr<UUID> agentId, std::shared_ptr<UUID> memoryId);
    virtual std::shared_ptr<Promise<object>> deleteAllAgentInternalMemories(std::shared_ptr<UUID> agentId, std::shared_ptr<UUID> agentPerspectiveRoomId);
    virtual std::shared_ptr<Promise<any>> updateAgentInternalMemory(std::shared_ptr<UUID> agentId, std::shared_ptr<UUID> memoryId, any memoryData);
    virtual std::shared_ptr<Promise<object>> deleteGroupMemory(std::shared_ptr<UUID> serverId, std::shared_ptr<UUID> memoryId);
    virtual std::shared_ptr<Promise<object>> clearGroupChat(std::shared_ptr<UUID> serverId);
    MemoryService(std::shared_ptr<ApiClientConfig> config);
};

#endif
