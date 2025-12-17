#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

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
    std::optional<std::string> branch;
    std::optional<std::string> tag;
};

struct UpgradePluginOptions {
    std::optional<std::string> apiKey;
    std::optional<bool> skipTests;
    std::optional<bool> skipValidation;
};

struct GeneratePluginOptions {
    std::optional<std::string> apiKey;
    std::optional<bool> skipTests;
    std::optional<bool> skipValidation;
    std::optional<bool> skipPrompts;
    std::optional<std::string> specFile;
};

/**
 * Plugin registry interfaces
 */
struct PluginRegistryInfo {
    std::string repo;
    std::string version;
    std::string branch;
    std::string version;
    std::string branch;
    std::string repo;
    std::optional<std::string> v0;
    std::optional<std::string> v1;
    bool v0;
    bool v1;
};

struct PluginRegistry {
};

/**
 * Environment variable configuration
 */
struct EnvVarConfig {
    std::string type;
    std::string description;
    std::optional<bool> required;
    std::optional<std::string> default;
    std::optional<bool> sensitive;
};

/**
 * Plugin migration result
 */
struct MigrationResult {
    bool success;
    std::optional<std::string> branchName;
    std::optional<std::string> repoPath;
    std::optional<std::runtime_error> error;
};

/**
 * Plugin generation result
 */
struct GenerationResult {
    bool success;
    std::optional<std::string> pluginName;
    std::optional<std::string> pluginPath;
    std::optional<std::runtime_error> error;
};

/**
 * Directory information from detection
 */
struct DirectoryInfo {
    std::string type;
    bool hasPackageJson;
};

/**
 * Package.json dependencies
 */
using Dependencies = std::unordered_map<std::string, std::string>;

} // namespace elizaos
