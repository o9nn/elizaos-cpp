#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include "elizaos/core.hpp"
#include "generation.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Environment Manager Service for handling environment variable configuration
 * Follows the same pattern as TaskService and other services in the codebase
 */
class EnvManagerService {
public:
    std::future<EnvManagerService> start(IAgentRuntime runtime);
    std::future<void> initialize();
    std::future<void> scanPluginRequirements();
    std::future<void> scanCharacterSecrets(Record<string secrets, auto any>, EnvVarMetadata envVars);
    std::future<void> scanLoadedPlugins(EnvVarMetadata envVars);
    EnvVarConfig["type"] inferVariableType(const std::string& varName);
    std::variant<std::future<Record<string, EnvVarConfig>, null>> getEnvVarsForPlugin(const std::string& pluginName);
    std::variant<Promise<EnvVarMetadata, null>> getAllEnvVars();
    std::future<bool> updateEnvVar(const std::string& pluginName, const std::string& varName, const std::optional<EnvVarConfig>& updates);
    std::future<bool> hasMissingEnvVars();
    Array< getMissingEnvVars();
    Array< getGeneratableEnvVars();
    std::future<void> stop();
    std::future<void> stop(IAgentRuntime runtime);
};


} // namespace elizaos
