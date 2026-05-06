#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

struct TableSchema {
    std::string name;
    std::string pluginName;
    std::string sql;
    std::optional<std::vector<std::string>> dependencies;
    std::optional<std::string> fallbackSql;
};

/**
 * Todo plugin table definitions for the unified migration system
 */


} // namespace elizaos
