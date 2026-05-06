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
 * Update a repository's last fetched timestamp
 */
std::future<void> updateRepositoryLastFetched(const std::string& repoId, const std::string& timestamp);

/**
 * Ensure users exist in the database
 */
std::future<void> ensureUsersExist(Map<string userData, std::optional<string }>> { avatarUrl, std::optional<std::vector<std::string>> botUsers);

/**
 * Ensure labels exist in the database
 */

/**
 * Store pull request to label relationships
 */
std::future<void> storePRLabels(const std::string& prId, const std::vector<std::string>& labelIds);

/**
 * Store issue to label relationships
 */
std::future<void> storeIssueLabels(const std::string& issueId, const std::vector<std::string>& labelIds);

} // namespace elizaos
