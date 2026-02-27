#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "package.json.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct VersionInfo {
    std: version;
    std: source;
    std: timestamp;
    std: environment;
    double uptime;
    std::optional<std:> error;
};

/**
 * Gets version information using CLI-compatible logic
 */
VersionInfo getVersionInfo();

/**
 * Creates the version router for system version information
 */

} // namespace elizaos
