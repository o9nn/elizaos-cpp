#include "...package.json.hpp"
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



struct VersionInfo {
    std::string version;
    std::string source;
    std::string timestamp;
    std::string environment;
    double uptime;
    std::optional<std::string> error;
};

/**
 * Gets version information using CLI-compatible logic
 */
VersionInfo getVersionInfo();

/**
 * Creates the version router for system version information
 */

} // namespace elizaos
