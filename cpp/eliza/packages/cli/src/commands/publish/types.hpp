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
 * Package metadata interface for registry publication
 */
struct PackageMetadata {
    std::string name;
    std::string version;
    std::string description;
    std::string type;
    std::string platform;
    std::string runtimeVersion;
    std::string repository;
    std::vector<std::string> maintainers;
    std::string publishedAt;
    std::string publishedBy;
    std::unordered_map<std::string, std::string> dependencies;
    std::vector<std::string> tags;
    std::string license;
    std::optional<std::string> npmPackage;
    std::optional<std::string> githubRepo;
};

/**
 * Publish command options
 */
struct PublishOptions {
    std::optional<bool> npm;
    std::optional<bool> test;
    std::optional<bool> dryRun;
    std::optional<bool> skipRegistry;
};

/**
 * Credentials interface
 */
struct Credentials {
    std::string username;
    std::string token;
};

/**
 * Directory information from detection
 */
struct DirectoryInfo {
    std::string type;
    bool hasPackageJson;
};

/**
 * Registry settings interface
 */
struct RegistrySettings {
    std::string defaultRegistry;
    std::optional<{> publishConfig;
    std::string registry;
    std::string username;
    bool useNpm;
    std::string platform;
};

/**
 * Package.json structure with optional fields used in publishing
 */
struct PackageJson {
    std::string name;
    std::string version;
    std::optional<std::string> description;
    std::optional<std::string> type;
    std::optional<'node' | 'browser' | 'universal'> platform;
    std::optional<{> repository;
    std::string type;
    std::string url;
    std::optional<std::vector<std::string>> maintainers;
    std::optional<std::unordered_map<std::string, std::string>> dependencies;
    std::optional<std::vector<std::string>> keywords;
    std::optional<std::string> license;
    std::optional<std::string> author;
    std::optional<{> bugs;
    std::string url;
    std::optional<std::string> npmPackage;
    std::optional<std::string> githubRepo;
    std::optional<'plugin' | 'project'> packageType;
    std::optional<{> agentConfig;
    std::string pluginType;
    std::unordered_map<std::string, std::any> pluginParameters;
    std::optional<{> eliza;
    std::string type;
};

/**
 * Placeholder replacement configuration
 */
struct PlaceholderReplacement {
    () => boolean check;
    () => void replace;
};

/**
 * Publish result from GitHub publishing
 */
using PublishResult = std::variant<bool, { success: boolean>; prUrl?: string };

} // namespace elizaos
