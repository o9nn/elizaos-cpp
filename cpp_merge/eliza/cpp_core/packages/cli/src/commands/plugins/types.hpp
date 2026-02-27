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

/**
 * Plugin command options for different subcommands
 */
struct ListPluginsOptions {
    std::optional<bool> all;
    std::optional<bool> v0;
};

struct AddPluginOptions {
    std::optional<bool> skipEnvPrompt;
    std::optional<bool> skipVerification;
    std::optional<std:> branch;
    std::optional<std:> tag;
};

struct UpgradePluginOptions {
    std::optional<std:> apiKey;
    std::optional<bool> skipTests;
    std::optional<bool> skipValidation;
};

struct GeneratePluginOptions {
    std::optional<std:> apiKey;
    std::optional<bool> skipTests;
    std::optional<bool> skipValidation;
    std::optional<bool> skipPrompts;
    std::optional<std:> specFile;
};

/**
 * Plugin registry interfaces
 */
struct PluginRegistryInfo {
    std::optional<{> git;
    std: repo;
    std::optional<{> v0;
    std: version;
    std: branch;
    std::optional<{> v1;
    std: version;
    std: branch;
    std::optional<{> npm;
    std: repo;
    std::optional<std:> v0;
    std::optional<std:> v1;
    { supports;
    bool v0;
    bool v1;
};

struct PluginRegistry {
    std::unordered_map<std:, PluginRegistryInfo> registry;
};

/**
 * Environment variable configuration
 */
struct EnvVarConfig {
    std: type;
    std: description;
    std::optional<bool> required;
    std::optional<std:> default;
    std::optional<bool> sensitive;
};

/**
 * Plugin migration result
 */
struct MigrationResult {
    bool success;
    std::optional<std:> branchName;
    std::optional<std:> repoPath;
    std::optional<Error> error;
};

/**
 * Plugin generation result
 */
struct GenerationResult {
    bool success;
    std::optional<std:> pluginName;
    std::optional<std:> pluginPath;
    std::optional<Error> error;
};

/**
 * Directory information from detection
 */
struct DirectoryInfo {
    std: type;
    bool hasPackageJson;
};

/**
 * Package.json dependencies
 */
using Dependencies = std::unordered_map<std:, std:>;

} // namespace elizaos
