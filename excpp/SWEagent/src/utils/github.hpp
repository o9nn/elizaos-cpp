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
    InvalidGithubURL(const std::string& message);
};

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

/**
 * Parse GitHub repository URL
 */

/**
 * Get GitHub issue data
 */
std::future<std::any> getGhIssueData(const std::string& issueUrl, std::string token = "");

/**
 * Get problem statement from GitHub issue
 */
std::future<std::string> getProblemStatementFromGithubIssue(const std::string& owner, const std::string& repo, const std::string& issueNumber, std::string token = "");

/**
 * Get associated commit URLs for an issue
 */

} // namespace elizaos
