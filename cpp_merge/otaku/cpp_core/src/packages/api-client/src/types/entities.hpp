#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct Entity {
    UUID id;
    UUID agentId;
    std::vector<std::string> names;
};

struct EntityCreateParams {
    UUID id;
    UUID agentId;
    std::optional<std::vector<std::string>> names;
};

struct EntityUpdateParams {
    std::optional<std::vector<std::string>> names;
    std::optional<UUID> agentId;
};


} // namespace elizaos
