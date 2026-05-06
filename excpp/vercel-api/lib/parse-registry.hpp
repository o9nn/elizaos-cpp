#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "types.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Registry configuration

// Helper function to safely fetch JSON
    // Only filter if data is a record-like object

// Parse GitHub reference

// Get GitHub branches
std::future<void> getGitHubBranches(const std::string& owner, const std::string& repo, Octokit octokit);

// Fetch package.json from GitHub

// Get latest Git tags
std::future<void> getLatestGitTags(const std::string& owner, const std::string& repo, Octokit octokit);

// Inspect NPM package
std::future<VersionInfo['npm']> inspectNpm(const std::string& pkgName);

// Guess NPM name from JS name
std::string guessNpmName(const std::string& jsName);

// Process a single repository

// Main function to parse registry


} // namespace elizaos
