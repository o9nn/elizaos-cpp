#include "base.hpp"
#include "elizaos/core.hpp"
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

;
;

struct Memory {
    UUID id;
    UUID agentId;
    std::optional<UUID> roomId;
    std::string type;
    std::any content;
    std::optional<std::vector<double>> embedding;
    Date createdAt;
    Date updatedAt;
    std::optional<std::unordered_map<std::string, std::any>> metadata;
};


struct Room {
    UUID id;
    UUID agentId;
    std::string name;
    std::optional<std::string> type;
    Date createdAt;
    Date updatedAt;
    std::optional<std::unordered_map<std::string, std::any>> metadata;
};


interface MemoryParams extends PaginationParams {
  type?: string;
  search?: string;
  from?: Date | string;
  to?: Date | string;
}

struct MemoryUpdateParams {
    std::optional<std::any> content;
    std::optional<std::unordered_map<std::string, std::any>> metadata;
};


struct RoomCreateParams {
    std::string name;
    std::optional<std::string> type;
    std::optional<std::unordered_map<std::string, std::any>> metadata;
};


struct WorldCreateParams {
    UUID serverId;
    std::string name;
    std::optional<std::string> description;
};


} // namespace elizaos
