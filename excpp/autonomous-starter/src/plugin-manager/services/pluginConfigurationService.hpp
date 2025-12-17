#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "elizaos/core.hpp"
#include "types.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



class PluginConfigurationService {
public:
    PluginConfigurationService(IAgentRuntime runtime);
    static std::future<PluginConfigurationService> start(IAgentRuntime runtime);
    std::future<void> initialize();
    Promise< parsePluginRequirements(const std::string& pluginPath);
    std::future<std::unordered_map<std::string, std::string>> getPluginConfiguration(const std::string& pluginName);
    std::future<void> setPluginConfiguration(const std::string& pluginName, const std::unordered_map<std::string, std::string>& config, std::optional<std::unordered_map<std::string, PluginEnvironmentVariable>> metadata);
    Promise< validateConfiguration(const std::string& pluginName, const std::unordered_map<std::string, std::string>& config, const std::vector<PluginEnvironmentVariable>& requirements);
    std::future<PluginConfigurationRequest> generateConfigurationRequest(const std::string& pluginName, const std::string& pluginPath);
    std::future<std::unordered_map<std::string, std::string>> applyConfigurationToEnvironment(const std::string& pluginName);
    std::future<bool> hasValidConfiguration(const std::string& pluginName, const std::string& pluginPath);
    std::future<std::vector<uint8_t>> getOrCreateEncryptionKey();
    std::future<std::string> encrypt(const std::string& text);
    std::future<std::string> decrypt(const std::string& text);
    bool isSensitiveKey(const std::string& key);
    std::future<void> loadConfigurations();
    std::future<void> saveConfigurations();
    std::future<void> stop();
};


} // namespace elizaos
