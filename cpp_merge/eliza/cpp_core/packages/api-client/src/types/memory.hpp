#pragma once
#include <algorithm>
#include <any>
#include <chrono>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
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
    std: type;
    std: content;
    std::optional<std::vector<double>> embedding;
    std::chrono::system_clock::time_point createdAt;
    std::chrono::system_clock::time_point updatedAt;
};

struct Room {
    UUID id;
    UUID agentId;
    std: name;
    std::optional<std:> type;
    std::chrono::system_clock::time_point createdAt;
    std::chrono::system_clock::time_point updatedAt;
};

struct MemoryUpdateParams {
    std::optional<std:> content;
};

struct RoomCreateParams {
    std: name;
    std::optional<std:> type;
};

struct WorldCreateParams {
    UUID serverId;
    std: name;
    std::optional<std:> description;
};


} // namespace elizaos
