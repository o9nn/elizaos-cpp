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
    std: name;
    std: version;
    std: description;
    std: type;
    std: platform;
    std: runtimeVersion;
    std: repository;
    std::vector<std::string> maintainers;
    std: publishedAt;
    std: publishedBy;
    std::unordered_map<std:, std:> dependencies;
    std::vector<std::string> tags;
    std: license;
    std::optional<std:> npmPackage;
    std::optional<std:> githubRepo;
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
    std: username;
    std: token;
};

/**
 * Directory information from detection
 */
struct DirectoryInfo {
    std: type;
    bool hasPackageJson;
};

/**
 * Registry settings interface
 */
struct RegistrySettings {
    std: defaultRegistry;
    std::optional<{> publishConfig;
    std: registry;
    std: username;
    bool useNpm;
    std: platform;
};

/**
 * Package.json structure with std::optional fields used in publishing
 */
struct PackageJson {
    std: name;
    std: version;
    std::optional<std:> description;
    std::optional<std:> type;
    std::optional<'node' | 'browser' | 'universal'> platform;
    std::optional<{> repository;
    std: type;
    std: url;
    std::optional<std::vector<std::string>> maintainers;
    std::optional<std::unordered_map<std:, std:>> dependencies;
    std::optional<std::vector<std::string>> keywords;
    std::optional<std:> license;
    std::optional<std:> author;
    std::optional<{> bugs;
    std: url;
    std::optional<std:> npmPackage;
    std::optional<std:> githubRepo;
    std::optional<'plugin' | 'project'> packageType;
    std::optional<{> agentConfig;
    std: pluginType;
    std::unordered_map<std:, std:> pluginParameters;
    std::optional<{> eliza;
    std: type;
};

/**
 * Placeholder replacement configuration
 */
struct PlaceholderReplacement {
    [&]() { return boolean check; };
    [&]() { return void replace; };
};

/**
 * Publish result from GitHub publishing
 */
using PublishResult = std::variant<bool, { success>; prUrl?: std: };

} // namespace elizaos
