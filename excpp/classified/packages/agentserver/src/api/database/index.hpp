#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct ColumnInfo {
    std::string name;
    std::string type;
    bool nullable;
    std::optional<std::string> defaultValue;
    std::optional<bool> primaryKey;
};

/**
 * Database router factory function for the main API router
 */

} // namespace elizaos
