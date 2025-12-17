#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".types.hpp"
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Simplified plugin configuration service for basic environment variable checking
 */
class PluginConfigurationService {
public:
    PluginConfigurationService(IAgentRuntime runtime);
    static std::future<PluginConfigurationService> start(IAgentRuntime runtime);
    std::future<void> initialize();
    Promise< parsePluginRequirements(const std::string& pluginPath);
    std::unordered_map<std::string, std::string> getPluginConfiguration(const std::string& pluginName);
    std::future<bool> hasValidConfiguration(const std::string& pluginName, const std::string& pluginPath);
    std::future<std::vector<std::string>> getMissingEnvVars(const std::string& pluginName, const std::string& pluginPath);
      string, getPluginConfigurationStatus();
    Promise< scanForCommonEnvVars(const std::string& pluginPath);
    std::future<void> stop();
};


} // namespace elizaos
