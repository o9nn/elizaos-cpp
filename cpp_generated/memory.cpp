#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/api-client/src/services/memory.h"

std::shared_ptr<Promise<object>> MemoryService::getAgentMemories(std::shared_ptr<UUID> agentId, std::shared_ptr<MemoryParams> params)
{
    return this->get<object>(std::string("/api/memory/") + agentId + std::string("/memories"), object{
        object::pair{std::string("params"), std::string("params")}
    });
}

std::shared_ptr<Promise<object>> MemoryService::getRoomMemories(std::shared_ptr<UUID> agentId, std::shared_ptr<UUID> roomId, std::shared_ptr<MemoryParams> params)
{
    return this->get<object>(std::string("/api/memory/") + agentId + std::string("/rooms/") + roomId + std::string("/memories"), object{
        object::pair{std::string("params"), std::string("params")}
    });
}

std::shared_ptr<Promise<std::shared_ptr<Memory>>> MemoryService::updateMemory(std::shared_ptr<UUID> agentId, std::shared_ptr<UUID> memoryId, std::shared_ptr<MemoryUpdateParams> params)
{
    return this->patch<std::shared_ptr<Memory>>(std::string("/api/memory/") + agentId + std::string("/memories/") + memoryId + string_empty, params);
}

std::shared_ptr<Promise<object>> MemoryService::clearAgentMemories(std::shared_ptr<UUID> agentId)
{
    return this->delete<object>(std::string("/api/memory/") + agentId + std::string("/memories"));
}

std::shared_ptr<Promise<object>> MemoryService::clearRoomMemories(std::shared_ptr<UUID> agentId, std::shared_ptr<UUID> roomId)
{
    return this->delete<object>(std::string("/api/memory/") + agentId + std::string("/memories/all/") + roomId + string_empty);
}

std::shared_ptr<Promise<object>> MemoryService::listAgentRooms(std::shared_ptr<UUID> agentId)
{
    return this->get<object>(std::string("/api/memory/") + agentId + std::string("/rooms"));
}

std::shared_ptr<Promise<std::shared_ptr<Room>>> MemoryService::getRoom(std::shared_ptr<UUID> agentId, std::shared_ptr<UUID> roomId)
{
    return this->get<std::shared_ptr<Room>>(std::string("/api/memory/") + agentId + std::string("/rooms/") + roomId + string_empty);
}

std::shared_ptr<Promise<std::shared_ptr<Room>>> MemoryService::createRoom(std::shared_ptr<UUID> agentId, std::shared_ptr<RoomCreateParams> params)
{
    return this->post<std::shared_ptr<Room>>(std::string("/api/memory/") + agentId + std::string("/rooms"), params);
}

std::shared_ptr<Promise<object>> MemoryService::createWorldFromServer(std::shared_ptr<UUID> serverId, std::shared_ptr<WorldCreateParams> params)
{
    return this->post<object>(std::string("/api/memory/groups/") + serverId + string_empty, params);
}

std::shared_ptr<Promise<object>> MemoryService::deleteWorld(std::shared_ptr<UUID> serverId)
{
    return this->delete<object>(std::string("/api/memory/groups/") + serverId + string_empty);
}

std::shared_ptr<Promise<object>> MemoryService::clearWorldMemories(std::shared_ptr<UUID> serverId)
{
    return this->delete<object>(std::string("/api/memory/groups/") + serverId + std::string("/memories"));
}

std::shared_ptr<Promise<object>> MemoryService::deleteMemory(std::shared_ptr<UUID> agentId, std::shared_ptr<UUID> memoryId)
{
    return this->delete<object>(std::string("/api/memory/") + agentId + std::string("/memories/") + memoryId + string_empty);
}

std::shared_ptr<Promise<object>> MemoryService::getAgentInternalMemories(std::shared_ptr<UUID> agentId, std::shared_ptr<UUID> agentPerspectiveRoomId, boolean includeEmbedding)
{
    return this->get<object>(std::string("/api/memory/") + agentId + std::string("/rooms/") + agentPerspectiveRoomId + std::string("/memories"), object{
        object::pair{std::string("params"), object{
            object::pair{std::string("includeEmbedding"), std::string("includeEmbedding")}
        }}
    });
}

std::shared_ptr<Promise<object>> MemoryService::deleteAgentInternalMemory(std::shared_ptr<UUID> agentId, std::shared_ptr<UUID> memoryId)
{
    return this->delete<object>(std::string("/api/memory/") + agentId + std::string("/memories/") + memoryId + string_empty);
}

std::shared_ptr<Promise<object>> MemoryService::deleteAllAgentInternalMemories(std::shared_ptr<UUID> agentId, std::shared_ptr<UUID> agentPerspectiveRoomId)
{
    return this->delete<object>(std::string("/api/memory/") + agentId + std::string("/memories/all/") + agentPerspectiveRoomId + string_empty);
}

std::shared_ptr<Promise<any>> MemoryService::updateAgentInternalMemory(std::shared_ptr<UUID> agentId, std::shared_ptr<UUID> memoryId, any memoryData)
{
    return this->patch<any>(std::string("/api/memory/") + agentId + std::string("/memories/") + memoryId + string_empty, memoryData);
}

std::shared_ptr<Promise<object>> MemoryService::deleteGroupMemory(std::shared_ptr<UUID> serverId, std::shared_ptr<UUID> memoryId)
{
    return this->delete<object>(std::string("/api/messaging/central-channels/") + serverId + std::string("/messages/") + memoryId + string_empty);
}

std::shared_ptr<Promise<object>> MemoryService::clearGroupChat(std::shared_ptr<UUID> serverId)
{
    return this->delete<object>(std::string("/api/messaging/central-channels/") + serverId + std::string("/messages"));
}

MemoryService::MemoryService(std::shared_ptr<ApiClientConfig> config) : BaseApiClient(config) {
}

