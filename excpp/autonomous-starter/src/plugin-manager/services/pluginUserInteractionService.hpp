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
#include "pluginConfigurationService.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



class PluginUserInteractionService {
public:
    PluginUserInteractionService(IAgentRuntime runtime);
    static std::future<PluginUserInteractionService> start(IAgentRuntime runtime);
    std::future<void> initialize();
    std::future<ConfigurationDialog> initiateConfigurationDialog(PluginConfigurationRequest request, const std::string& userId);
    Promise< processUserResponse(const std::string& dialogId, const std::string& userId, const std::string& response);
    std::string generatePromptForVariable(PluginEnvironmentVariable varInfo);
    std::future<void> cancelDialog(const std::string& dialogId);
    std::vector<ConfigurationDialog> getActiveDialogs();
    ConfigurationDialog getDialogById(const std::string& dialogId);
    ConfigurationDialog getDialogByUser(const std::string& userId);
    Promise< validateUserResponse(PluginEnvironmentVariable varInfo, const std::string& value);
    std::future<void> applyDialogConfiguration(ConfigurationDialog dialog);
    std::future<void> createDialogMemory(ConfigurationDialog dialog, const std::string& userId);
    std::future<void> cleanupCompletedDialogs();
    std::future<void> stop();
};


} // namespace elizaos
