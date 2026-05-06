#pragma once
#include <algorithm>
#include <any>
#include <chrono>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use


// Extend the core service types with plugin manager service
  struct ServiceTypeRegistry {
    "PLUGIN_MANAGER" PLUGIN_MANAGER;
    "PLUGIN_CONFIGURATION" PLUGIN_CONFIGURATION;
    "PLUGIN_USER_INTERACTION" PLUGIN_USER_INTERACTION;
};

// Export service type constant

enum PluginStatus {
}

// Configuration-related types
struct PluginEnvironmentVariable {
    std::string name;
    std::string description;
    bool sensitive;
    bool required;
    std::optional<std::string> defaultValue;
    std::optional<std::string> pattern;
    std::optional<double> minLength;
    std::optional<double> maxLength;
    std::optional<std::vector<std::string>> enum;
};

struct PluginConfigurationRequest {
    std::string pluginName;
    std::vector<PluginEnvironmentVariable> requiredVars;
    std::vector<std::string> missingVars;
    std::vector<PluginEnvironmentVariable> optionalVars;
};

struct ConfigurationDialog {
    std::string id;
    std::string pluginName;
    std::string status;
    PluginConfigurationRequest request;
    std::optional<std::string> currentVariable;
    std::chrono::system_clock::time_point startedAt;
    std::optional<std::chrono::system_clock::time_point> completedAt;
};

struct PluginState {
    std::string id;
    std::string name;
    PluginStatus status;
    std::optional<Plugin> plugin;
    std::vector<std::string> missingEnvVars;
    std::vector<std::string> buildLog;
    std::optional<std::string> sourceCode;
    std::optional<std::any> packageJson;
    std::optional<std::string> error;
    double createdAt;
    std::optional<double> loadedAt;
    std::optional<double> unloadedAt;
    std::optional<std::string> version;
    std::optional<std::string> configurationStatus;
    std::optional<std::vector<PluginEnvironmentVariable>> requiredConfiguration;
    std::optional<std::vector<std::string>> configurationErrors;
};

struct PluginRegistry {
};

struct CreatePluginParams {
    std::string name;
    std::string description;
    std::vector<std::string> capabilities;
    std::optional<std::vector<std::string>> dependencies;
};

struct LoadPluginParams {
    std::string pluginId;
    std::optional<bool> force;
};

struct UnloadPluginParams {
    std::string pluginId;
};

struct PluginManagerConfig {
    std::optional<double> maxBuildAttempts;
    std::optional<double> buildTimeout;
    std::optional<std::string> pluginDirectory;
    std::optional<bool> enableHotReload;
};


} // namespace elizaos
