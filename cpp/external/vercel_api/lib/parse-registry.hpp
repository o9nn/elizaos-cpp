#pragma once

#include <algorithm>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <regex>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "types.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * Parsed Git reference
 */
struct ParsedGitRef {
    std::string owner;
    std::string repo;
};

/**
 * Octokit placeholder - represents GitHub API client
 */
class Octokit {
public:
    // Placeholder for GitHub API client
    Octokit() = default;
    explicit Octokit(const std::string& token) : token_(token) {}
private:
    std::string token_;
};

/**
 * Parse a GitHub reference string
 * @param gitRef - Git reference string (e.g., "github:owner/repo")
 * @return Parsed reference or nullopt if invalid
 */
std::optional<ParsedGitRef> parseGitRef(const std::string& gitRef);

/**
 * Get GitHub branches for a repository
 * @param owner - Repository owner
 * @param repo - Repository name
 * @param octokit - GitHub API client
 * @return Future with list of branch names
 */
std::future<std::vector<std::string>> getGitHubBranches(
    const std::string& owner, 
    const std::string& repo, 
    const Octokit& octokit);

/**
 * Package JSON information
 */
struct PackageInfo {
    std::string version;
    std::optional<std::string> coreRange;
};

/**
 * Fetch package.json from GitHub
 * @param owner - Repository owner
 * @param repo - Repository name
 * @param ref - Git reference (branch/tag)
 * @param octokit - GitHub API client
 * @return Future with package info
 */
std::future<std::optional<PackageInfo>> fetchPackageJSON(
    const std::string& owner, 
    const std::string& repo, 
    const std::string& ref, 
    const Octokit& octokit);

/**
 * Git tag information
 */
struct GitTagInfo {
    std::string repo;
    std::optional<std::string> v0;
    std::optional<std::string> v1;
};

/**
 * Get latest Git tags for a repository
 * @param owner - Repository owner
 * @param repo - Repository name
 * @param octokit - GitHub API client
 * @return Future with tag info
 */
std::future<GitTagInfo> getLatestGitTags(
    const std::string& owner, 
    const std::string& repo, 
    const Octokit& octokit);

/**
 * Inspect NPM package for version information
 * @param pkgName - NPM package name
 * @return Future with NPM info
 */
std::future<NpmInfo> inspectNpm(const std::string& pkgName);

/**
 * Guess NPM name from JS package name
 * @param jsName - JavaScript package name
 * @return Guessed NPM package name
 */
std::string guessNpmName(const std::string& jsName);

/**
 * Process a single repository
 * @param npmId - NPM package ID
 * @param gitRef - Git reference
 * @param octokit - GitHub API client
 * @return Future with tuple of npm ID and version info
 */
std::future<std::tuple<std::string, VersionInfo>> processRepo(
    const std::string& npmId, 
    const std::string& gitRef, 
    const Octokit& octokit);

/**
 * Parse the registry
 * @param githubToken - GitHub API token
 * @return Future with cached registry
 */
std::future<CachedRegistry> parseRegistry(const std::string& githubToken);

} // namespace elizaos
