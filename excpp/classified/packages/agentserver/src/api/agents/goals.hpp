#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".shared/response-utils.hpp"
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct Goal {
    std::string id;
    std::string name;
    std::string description;
    std::string status;
    double progress;
    std::string createdAt;
    std::string updatedAt;
};

/**
 * Agent goals management operations
 */

} // namespace elizaos
