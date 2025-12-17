#pragma once
#include <any>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Service type declarations for plugin manager
  struct ServiceTypeRegistry {
    std::string PLUGIN_MANAGER;
    std::string PLUGIN_CONFIGURATION;
    std::string REGISTRY;
};

enum PluginStatus {
}

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

struct PluginComponents {
    std::unordered_set<std::string> actions;
    std::unordered_set<std::string> providers;
    std::unordered_set<std::string> evaluators;
    std::unordered_set<std::string> services;
};

struct ComponentRegistration {
    std::string pluginId;
    std::string componentType;
    std::string componentName;
    double timestamp;
};

struct PluginState {
    std::string id;
    std::string name;
    PluginStatus status;
    std::optional<ElizaPlugin> plugin;
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
    std::optional<PluginComponents> components;
};

struct PluginRegistry {
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

struct InstallProgress {
    std::string phase;
    double progress;
    std::string message;
};

struct PluginMetadata {
    std::string name;
    std::string description;
    std::string author;
    std::string repository;
    std::vector<std::string> versions;
    std::string latestVersion;
    std::string runtimeVersion;
    std::string maintainer;
    std::optional<std::vector<std::string>> tags;
    std::optional<std::vector<std::string>> categories;
};


} // namespace elizaos
