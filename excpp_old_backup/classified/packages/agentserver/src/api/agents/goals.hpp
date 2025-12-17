#include ".shared/response-utils.hpp"
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



struct Goal {
    std::string id;
    std::string name;
    std::string description;
    'active' | 'completed' | 'paused' status;
    double progress;
    std::string createdAt;
    std::string updatedAt;
    std::optional<std::unordered_map<std::string, std::any>> metadata;
};

/**
 * Agent goals management operations
 */

} // namespace elizaos
