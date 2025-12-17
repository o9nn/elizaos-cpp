#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * Extract repository information from the current URL
 * Defaults to elizaos/eliza if not running on GitHub Pages
 */
void getRepositoryInfo();

/**
 * Get the full repository path (owner/repo)
 */
void getRepositoryPath();

/**
 * Get the GitHub repository URL
 */
void getGitHubUrl();

/**
 * Get the GitHub releases URL
 */
void getReleasesUrl();

/**
 * Get the GitHub API releases URL
 */
void getApiReleasesUrl();
} // namespace elizaos
