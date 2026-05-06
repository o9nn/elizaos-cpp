#pragma once
#include <any>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".lib/base-client.hpp"
#include ".types/memory.hpp"
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



class MemoryService {
public:
    Promise< getAgentMemories(UUID agentId, std::optional<MemoryParams> params);
    Promise< getRoomMemories(UUID agentId, UUID roomId, std::optional<MemoryParams> params);
    std::future<Memory> updateMemory(UUID agentId, UUID memoryId, MemoryUpdateParams params);
    Promise< clearAgentMemories(UUID agentId);
    Promise< clearRoomMemories(UUID agentId, UUID roomId);
    Promise< listAgentRooms(UUID agentId);
    std::future<Room> getRoom(UUID agentId, UUID roomId);
    std::future<Room> createRoom(UUID agentId, RoomCreateParams params);
    Promise< createWorldFromServer(UUID serverId, WorldCreateParams params);
    Promise< deleteWorld(UUID serverId);
    Promise< clearWorldMemories(UUID serverId);
    Promise< deleteMemory(UUID agentId, UUID memoryId);
    Promise< getAgentInternalMemories(UUID agentId, UUID agentPerspectiveRoomId, std::optional<bool> includeEmbedding);
    Promise< deleteAgentInternalMemory(UUID agentId, UUID memoryId);
    Promise< deleteAllAgentInternalMemories(UUID agentId, UUID agentPerspectiveRoomId);
    std::future<std::any> updateAgentInternalMemory(UUID agentId, UUID memoryId, const std::any& memoryData);
    Promise< deleteGroupMemory(UUID serverId, UUID memoryId);
    Promise< clearGroupChat(UUID serverId);
};


} // namespace elizaos
