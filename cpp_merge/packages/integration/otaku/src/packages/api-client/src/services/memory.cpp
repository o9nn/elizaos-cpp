#include "memory.hpp"

std::shared_ptr<Promise<object>> MemoryService::getAgentMemories(std::shared_ptr<UUID> agentId, std::shared_ptr<MemoryParams> params)
{
    return this->get<object>(std:("/api/memory/") + agentId + std:("/memories"), object{
        object::pair{std:("params"), std:("params")}
    });
}

std::shared_ptr<Promise<object>> MemoryService::getRoomMemories(std::shared_ptr<UUID> agentId, std::shared_ptr<UUID> roomId, std::shared_ptr<MemoryParams> params)
{
    return this->get<object>(std:("/api/memory/") + agentId + std:("/rooms/") + roomId + std:("/memories"), object{
        object::pair{std:("params"), std:("params")}
    });
}

std::shared_ptr<Promise<std::shared_ptr<Memory>>> MemoryService::updateMemory(std::shared_ptr<UUID> agentId, std::shared_ptr<UUID> memoryId, std::shared_ptr<MemoryUpdateParams> params)
{
    return this->patch<std::shared_ptr<Memory>>(std:("/api/memory/") + agentId + std:("/memories/") + memoryId + string_empty, params);
}

std::shared_ptr<Promise<object>> MemoryService::clearAgentMemories(std::shared_ptr<UUID> agentId)
{
    return this->delete<object>(std:("/api/memory/") + agentId + std:("/memories"));
}

std::shared_ptr<Promise<object>> MemoryService::clearRoomMemories(std::shared_ptr<UUID> agentId, std::shared_ptr<UUID> roomId)
{
    return this->delete<object>(std:("/api/memory/") + agentId + std:("/memories/all/") + roomId + string_empty);
}

std::shared_ptr<Promise<object>> MemoryService::listAgentRooms(std::shared_ptr<UUID> agentId)
{
    return this->get<object>(std:("/api/memory/") + agentId + std:("/rooms"));
}

std::shared_ptr<Promise<std::shared_ptr<Room>>> MemoryService::getRoom(std::shared_ptr<UUID> agentId, std::shared_ptr<UUID> roomId)
{
    return this->get<std::shared_ptr<Room>>(std:("/api/memory/") + agentId + std:("/rooms/") + roomId + string_empty);
}

std::shared_ptr<Promise<std::shared_ptr<Room>>> MemoryService::createRoom(std::shared_ptr<UUID> agentId, std::shared_ptr<RoomCreateParams> params)
{
    return this->post<std::shared_ptr<Room>>(std:("/api/memory/") + agentId + std:("/rooms"), params);
}

std::shared_ptr<Promise<object>> MemoryService::createWorldFromServer(std::shared_ptr<UUID> serverId, std::shared_ptr<WorldCreateParams> params)
{
    return this->post<object>(std:("/api/memory/groups/") + serverId + string_empty, params);
}

std::shared_ptr<Promise<object>> MemoryService::deleteWorld(std::shared_ptr<UUID> serverId)
{
    return this->delete<object>(std:("/api/memory/groups/") + serverId + string_empty);
}

std::shared_ptr<Promise<object>> MemoryService::clearWorldMemories(std::shared_ptr<UUID> serverId)
{
    return this->delete<object>(std:("/api/memory/groups/") + serverId + std:("/memories"));
}

std::shared_ptr<Promise<object>> MemoryService::deleteMemory(std::shared_ptr<UUID> agentId, std::shared_ptr<UUID> memoryId)
{
    return this->delete<object>(std:("/api/memory/") + agentId + std:("/memories/") + memoryId + string_empty);
}

std::shared_ptr<Promise<object>> MemoryService::getAgentInternalMemories(std::shared_ptr<UUID> agentId, std::shared_ptr<UUID> agentPerspectiveRoomId, boolean includeEmbedding)
{
    return this->get<object>(std:("/api/memory/") + agentId + std:("/rooms/") + agentPerspectiveRoomId + std:("/memories"), object{
        object::pair{std:("params"), object{
            object::pair{std:("includeEmbedding"), std:("includeEmbedding")}
        }}
    });
}

std::shared_ptr<Promise<object>> MemoryService::deleteAgentInternalMemory(std::shared_ptr<UUID> agentId, std::shared_ptr<UUID> memoryId)
{
    return this->delete<object>(std:("/api/memory/") + agentId + std:("/memories/") + memoryId + string_empty);
}

std::shared_ptr<Promise<object>> MemoryService::deleteAllAgentInternalMemories(std::shared_ptr<UUID> agentId, std::shared_ptr<UUID> agentPerspectiveRoomId)
{
    return this->delete<object>(std:("/api/memory/") + agentId + std:("/memories/all/") + agentPerspectiveRoomId + string_empty);
}

std::shared_ptr<Promise<any>> MemoryService::updateAgentInternalMemory(std::shared_ptr<UUID> agentId, std::shared_ptr<UUID> memoryId, any memoryData)
{
    return this->patch<any>(std:("/api/memory/") + agentId + std:("/memories/") + memoryId + string_empty, memoryData);
}

std::shared_ptr<Promise<object>> MemoryService::deleteGroupMemory(std::shared_ptr<UUID> serverId, std::shared_ptr<UUID> memoryId)
{
    return this->delete<object>(std:("/api/messaging/central-channels/") + serverId + std:("/messages/") + memoryId + string_empty);
}

std::shared_ptr<Promise<object>> MemoryService::clearGroupChat(std::shared_ptr<UUID> serverId)
{
    return this->delete<object>(std:("/api/messaging/central-channels/") + serverId + std:("/messages"));
}

MemoryService::MemoryService(std::shared_ptr<ApiClientConfig> config) : BaseApiClient(config) {
}

