#pragma once
#include <any>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "base.hpp"
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct Memory {
    UUID id;
    UUID agentId;
    std::optional<UUID> roomId;
    std::string type;
    std::any content;
    std::optional<std::vector<double>> embedding;
    Date createdAt;
    Date updatedAt;
};

struct Room {
    UUID id;
    UUID agentId;
    std::string name;
    std::optional<std::string> type;
    Date createdAt;
    Date updatedAt;
};

struct MemoryUpdateParams {
    std::optional<std::any> content;
};

struct RoomCreateParams {
    std::string name;
    std::optional<std::string> type;
};

struct WorldCreateParams {
    UUID serverId;
    std::string name;
    std::optional<std::string> description;
};


} // namespace elizaos
