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

// Define our own TableSchema type to avoid import issues during build
struct TableSchema {
    std::string name;
    std::string pluginName;
    std::string sql;
    std::optional<std::string> fallbackSql;
    std::optional<std::vector<std::string>> dependencies;
};

/**
 * Goals plugin table definitions for the unified migration system
 */

struct TaskMetadata {
    std::optional<std::string> dueDate;
    std::optional<double> streak;
    std::optional<bool> completedToday;
    std::optional<std::string> lastReminderSent;
    std::optional<double> pointsAwarded;
    std::optional<std::string> completedAt;
};


} // namespace elizaos
