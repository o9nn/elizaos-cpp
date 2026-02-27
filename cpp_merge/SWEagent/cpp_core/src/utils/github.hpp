#pragma once
#include <any>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * GitHub utilities
 * Converted from sweagent/utils/github.py
 */

class InvalidGithubURL {
public:
    InvalidGithubURL(const std:& message);
};

/**
 * Check if URL is a GitHub repository URL
 */
bool isGithubRepoUrl(const std:& dataPath);

/**
 * Check if URL is a GitHub issue URL
 */
bool isGithubIssueUrl(const std:& dataPath);

/**
 * Parse GitHub issue URL
 */
void parseGhIssueUrl(const std:& issueUrl); {

/**
 * Parse GitHub repository URL
 */
void parseGhRepoUrl(const std:& repoUrl); {

/**
 * Get GitHub issue data
 */
std::future<std:> getGhIssueData(const std:& issueUrl, std: = '' token);

/**
 * Get problem statement from GitHub issue
 */
std::future<std:> getProblemStatementFromGithubIssue(const std:& owner, const std:& repo, const std:& issueNumber, std: = '' token);

/**
 * Get associated commit URLs for an issue
 */

} // namespace elizaos
