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



struct Todo {
    std::string id;
    std::string task;
    bool completed;
    std::string priority;
    std::optional<std::string> dueDate;
    std::string createdAt;
    std::optional<std::string> completedAt;
};

/**
 * Agent todos management operations
 */

} // namespace elizaos
