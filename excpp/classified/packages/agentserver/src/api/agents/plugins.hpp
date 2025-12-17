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



struct PluginConfig {
    std::string id;
    std::string name;
    std::string displayName;
    bool enabled;
    std::string version;
    std::string description;
};

/**
 * Agent plugin configuration management
 */

} // namespace elizaos
