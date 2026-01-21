#include ".utils/github.hpp"
#include ".utils/log.hpp"
#include "deployment.hpp"
#include "runtime.hpp"
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
 * Repository management
 * Converted from sweagent/environment/repo.py
 */

/**
 * Repository protocol interface
 */
struct Repo {
    std::string baseCommit;
    std::string repoName;
};

/**
 * Get git reset commands
 */
std::vector<std::string> getGitResetCommands(const std::string& baseCommit);

/**
 * Pre-existing repository configuration
 */

using PreExistingRepoConfig = z.infer<typeof PreExistingRepoConfigSchema>;

class PreExistingRepo implements Repo {
  repoName: string;
  baseCommit: string;
  reset: boolean;

  constructor(config: PreExistingRepoConfig) {
    this.repoName = config.repoName;
    this.baseCommit = config.baseCommit;
    this.reset = config.reset;
  }

    return getGitResetCommands(this.baseCommit);

/**
 * Local repository configuration
 */

using LocalRepoConfig = z.infer<typeof LocalRepoConfigSchema>;

class LocalRepo implements Repo {
  path: string;
  baseCommit: string;
  repoName: string;

  constructor(config: LocalRepoConfig) {
    this.path = config.path;
    this.baseCommit = config.baseCommit;
    this.repoName = path.basename(this.path).replace(' ', '-').replace("'", ''); // Match Python sanitization
  }

    // Check if repo is dirty (has uncommitted changes)
    // Note: This is a simplified check - Python uses GitPython for this

    // Upload repository to deployment

    // Change permissions

    return getGitResetCommands(this.baseCommit);

/**
 * GitHub repository configuration
 */

using GithubRepoConfig = z.infer<typeof GithubRepoConfigSchema>;

class GithubRepo implements Repo {
  githubUrl: string;
  baseCommit: string;
  cloneTimeout: number;
  repoName: string;
  constructor(config: GithubRepoConfig) {
    // Handle short form github URLs (org/repo)
    if (config.githubUrl.split('/').length === 2 && !config.githubUrl.includes('://')) {
      this.githubUrl = `https://github.com/${config.githubUrl}`;
    } else {
      this.githubUrl = config.githubUrl;
    }

    this.baseCommit = config.baseCommit;
    this.cloneTimeout = config.cloneTimeout;

    const parsed = parseGhRepoUrl(this.githubUrl);
    this.repoName = `${parsed.owner}__${parsed.repo}`;
  }

    // Check if @ already in URL

    // Insert token into URL for authentication

    // Execute git commands directly in the deployment (matches Python implementation)

    return getGitResetCommands(this.baseCommit);

/**
 * Union type for all repo configurations
 */

using RepoConfig = z.infer<typeof RepoConfigSchema>;

/**
 * Factory function to create repo from simplified input
 */
Repo repoFromSimplifiedInput(const std::string& input, string = 'HEAD' baseCommit, 'local' | 'github' | 'preexisting' | 'auto' = 'auto' type);

} // namespace elizaos
