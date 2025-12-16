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



// Custom levels from @elizaos/core logger

/**
 * Defines a type `LogLevel` as the keys of the `LOG_LEVELS` object.
 */
using LogLevel = std::variant<keyof typeof LOG_LEVELS, 'all'>;

/**
 * Represents a log entry with specific properties.
 */
struct LogEntry {
    double level;
    double time;
    std::string msg;
};

/**
 * Logging management endpoints
 */

} // namespace elizaos
