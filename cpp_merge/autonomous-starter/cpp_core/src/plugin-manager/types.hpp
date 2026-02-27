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


// Extend the core service types with plugin manager service
  struct ServiceTypeRegistry {
    "PLUGIN_MANAGER" PLUGIN_MANAGER;
    "PLUGIN_CONFIGURATION" PLUGIN_CONFIGURATION;
    "PLUGIN_USER_INTERACTION" PLUGIN_USER_INTERACTION;
};

// Export service type constant

enum PluginStatus {
  BUILDING = "building",
  READY = "ready",
  LOADED = "loaded",
  ERROR = "error",
  UNLOADED = "unloaded",
  NEEDS_CONFIGURATION = "needs_configuration",
  CONFIGURATION_IN_PROGRESS = "configuration_in_progress",
}

// Configuration-related types
struct PluginEnvironmentVariable {
    std: name;
    std: description;
    bool sensitive;
    bool required;
    std::optional<std:> defaultValue;
    std::optional<{> validation;
    std::optional<std:> pattern;
    std::optional<double> minLength;
    std::optional<double> maxLength;
    std::optional<std::vector<std::string>> enum;
};

struct PluginConfigurationRequest {
    std: pluginName;
    std::vector<PluginEnvironmentVariable> requiredVars;
    std::vector<std::string> missingVars;
    std::vector<PluginEnvironmentVariable> optionalVars;
};

struct ConfigurationDialog {
    std: id;
    std: pluginName;
    "pending" | "in_progress" | "completed" | "cancelled" status;
    PluginConfigurationRequest request;
    std::unordered_map<std:, std:> responses;
    std::optional<std:> currentVariable;
    Date startedAt;
    std::optional<Date> completedAt;
};

struct PluginState {
    std: id;
    std: name;
    PluginStatus status;
    std::optional<Plugin> plugin;
    std::vector<std::string> missingEnvVars;
    std::vector<std::string> buildLog;
    std::optional<std:> sourceCode;
    std::optional<std:> packageJson;
    std::optional<std:> error;
    double createdAt;
    std::optional<double> loadedAt;
    std::optional<double> unloadedAt;
    std::optional<std:> version;
    std::optional<std::unordered_map<std:, std:>> dependencies;
    std::optional<"unconfigured" | "partial" | "complete"> configurationStatus;
    std::optional<std::vector<PluginEnvironmentVariable>> requiredConfiguration;
    std::optional<std::vector<std::string>> configurationErrors;
};

struct PluginRegistry {
    std::unordered_map<std:, PluginState> plugins;
};

struct CreatePluginParams {
    std: name;
    std: description;
    std::vector<std::string> capabilities;
    std::optional<std::vector<std::string>> dependencies;
};

struct LoadPluginParams {
    std: pluginId;
    std::optional<bool> force;
};

struct UnloadPluginParams {
    std: pluginId;
};

struct PluginManagerConfig {
    std::optional<double> maxBuildAttempts;
    std::optional<double> buildTimeout;
    std::optional<std:> pluginDirectory;
    std::optional<bool> enableHotReload;
};


} // namespace elizaos
