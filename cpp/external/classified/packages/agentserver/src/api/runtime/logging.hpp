#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Custom levels from @elizaos/core logger

/**
 * Defines a type `LogLevel` as the keys of the `LOG_LEVELS` object.
 */
using LogLevel = std::variant<keyof typeof LOG_LEVELS, std::string>;

/**
 * Represents a log entry with specific properties.
 */
struct LogEntry {
    std::variant<double, std::string> level;
    double time;
    std::optional<std::string> msg;
    std::optional<std::string> message;
    std::optional<std::string> agentName;
    std::optional<std::string> agentId;
};

/**
 * Logging management endpoints
 */

} // namespace elizaos
