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



struct AgentSettings {
    bool autonomy;
    { capabilities;
    bool shellAccess;
    bool webAccess;
    bool visionAccess;
    bool speechToText;
    bool textToSpeech;
    std::string modelProvider;
    std::unordered_map<std::string, std::string> apiKeys;
};

/**
 * Agent settings management operations
 */

} // namespace elizaos
