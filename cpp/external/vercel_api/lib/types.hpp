#pragma once

#include <any>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

// Types for the registry API
using RawRegistry = std::unordered_map<std::string, std::string>; // <npmName> → "github:owner/repo"

/**
 * Git repository information
 */
struct GitInfo {
    std::string repo;
    std::optional<std::string> version;
    std::optional<std::string> branch;
};

/**
 * NPM package information
 */
struct NpmInfo {
    std::optional<std::string> repo;
    std::optional<std::string> v0;
    std::optional<std::string> v1;
};

/**
 * Version support flags
 */
struct VersionSupport {
    bool v0 = false;
    bool v1 = false;
};

/**
 * Version information for a package
 */
struct VersionInfo {
    std::optional<GitInfo> git;
    std::optional<GitInfo> v0;
    std::optional<GitInfo> v1;
    std::optional<NpmInfo> npm;
    VersionSupport supports;
};

/**
 * Cached registry data
 */
struct CachedRegistry {
    std::string lastUpdatedAt;
    std::unordered_map<std::string, VersionInfo> registry;
};

/**
 * NPM package metadata
 */
struct NpmPackageMetadata {
    std::optional<std::unordered_map<std::string, std::any>> versions;
};

} // namespace elizaos
