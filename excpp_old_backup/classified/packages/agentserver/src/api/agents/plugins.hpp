#include ".shared/response-utils.hpp"
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



struct PluginConfig {
    std::string id;
    std::string name;
    std::string displayName;
    bool enabled;
    std::string version;
    std::string description;
    std::unordered_map<std::string, std::any> config;
};

/**
 * Agent plugin configuration management
 */

} // namespace elizaos
