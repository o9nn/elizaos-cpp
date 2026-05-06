#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Service responsible for progressive plugin registration and management.
 * Handles dynamic plugin loading based on capability unlocks.
 */
class ProgressivePluginService {
public:
    ProgressivePluginService(std::optional<IAgentRuntime> runtime, std::optional<Record<string> availablePlugins, auto Plugin[]>);
    void setAvailablePlugins(Record<string plugins, auto Plugin[]>);
    std::future<void> registerCapabilityPlugins(const std::string& capability);
    bool hasPluginsForCapability(const std::string& capability);
    std::vector<Plugin> getPluginsForCapability(const std::string& capability);
    bool isPluginRegistered(const std::string& pluginName);
    std::vector<std::string> getAvailableCapabilities();
    std::future<ProgressivePluginService> start(IAgentRuntime runtime);
    std::future<void> stop();

private:
    std::unordered_map<std::string, std::vector<Plugin>> availablePlugins_;
};


} // namespace elizaos
