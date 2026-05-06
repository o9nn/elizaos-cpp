#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".types/index.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



class ConfigManager {
public:
    GatewayConfig loadFromFile(const std::string& filePath);
    std::string substituteEnvVars(const std::string& content);
};

      // If no value and no default, return empty string or keep original


} // namespace elizaos
