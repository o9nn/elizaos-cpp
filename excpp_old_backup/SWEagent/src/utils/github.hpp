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
 * GitHub utilities
 * Converted from sweagent/utils/github.py
 */

class InvalidGithubURL extends Error {
  constructor(message: string) {
    super(message);
    this.name = 'InvalidGithubURL';
  }

/**
 * Check if URL is a GitHub repository URL
 */
bool isGithubRepoUrl(const std::string& dataPath);

/**
 * Check if URL is a GitHub issue URL
 */
bool isGithubIssueUrl(const std::string& dataPath);

/**
 * Parse GitHub issue URL
 */
void parseGhIssueUrl(const std::string& issueUrl); {

/**
 * Parse GitHub repository URL
 */
void parseGhRepoUrl(const std::string& repoUrl); {

/**
 * Get GitHub issue data
 */
std::future<std::any> getGhIssueData(const std::string& issueUrl, string = '' token);

/**
 * Get problem statement from GitHub issue
 */
std::future<std::string> getProblemStatementFromGithubIssue(const std::string& owner, const std::string& repo, const std::string& issueNumber, string = '' token);

/**
 * Get associated commit URLs for an issue
 */

} // namespace elizaos
