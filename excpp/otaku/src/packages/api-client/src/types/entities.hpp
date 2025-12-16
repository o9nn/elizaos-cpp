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

struct Entity {
    UUID id;
    UUID agentId;
    std::vector<std::string> names;
    std::optional<std::unordered_map<std::string, std::any>> metadata;
};


struct EntityCreateParams {
    UUID id;
    UUID agentId;
    std::optional<std::vector<std::string>> names;
    std::optional<std::unordered_map<std::string, std::any>> metadata;
};


struct EntityUpdateParams {
    std::optional<std::vector<std::string>> names;
    std::optional<std::unordered_map<std::string, std::any>> metadata;
    std::optional<UUID> agentId;
};



} // namespace elizaos
