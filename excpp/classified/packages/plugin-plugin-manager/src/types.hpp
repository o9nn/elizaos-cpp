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

;

// Service type declarations for plugin manager
declare module '@elizaos/core' {
  struct ServiceTypeRegistry {
    'plugin_manager' PLUGIN_MANAGER;
    'plugin_configuration' PLUGIN_CONFIGURATION;
    'registry' REGISTRY;
};

}

const PluginManagerServiceType = {
  PLUGIN_MANAGER: 'plugin_manager' as ServiceTypeName,
  PLUGIN_CONFIGURATION: 'plugin_configuration' as ServiceTypeName,
  REGISTRY: 'registry' as ServiceTypeName,
} as const;

enum PluginStatus {
  BUILDING = 'building',
  READY = 'ready',
  LOADED = 'loaded',
  ERROR = 'error',
  UNLOADED = 'unloaded',
  NEEDS_CONFIGURATION = 'needs_configuration',
}

struct PluginEnvironmentVariable {
    std::string name;
    std::string description;
    bool sensitive;
    bool required;
    std::optional<std::string> defaultValue;
    std::optional<{> validation;
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
    std::unordered_map<std::string, std::unordered_set<(params: any) => Promise<void>>> eventHandlers;
};


struct ComponentRegistration {
    std::string pluginId;
    'action' | 'provider' | 'evaluator' | 'service' | 'eventHandler' componentType;
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
    std::optional<std::unordered_map<std::string, std::string>> dependencies;
    std::optional<'unconfigured' | 'partial' | 'complete'> configurationStatus;
    std::optional<std::vector<PluginEnvironmentVariable>> requiredConfiguration;
    std::optional<std::vector<std::string>> configurationErrors;
    std::optional<PluginComponents> components;
};


struct PluginRegistry {
    std::unordered_map<std::string, PluginState> plugins;
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
    'downloading' | 'extracting' | 'installing' | 'validating' | 'complete' phase;
    number; // 0-100 progress;
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
