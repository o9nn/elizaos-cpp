#pragma once
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
 * Get PR data and file paths for a contributor
 */
std::future<void> getContributorPRs(const std::string& username, PaginatedQueryParams = {} params);

/**
 * Get aggregated pull request metrics for a contributor
 */
std::future<void> getContributorPRMetrics(const std::string& username, QueryParams = {} params);

/**
 * Get aggregated issue metrics for a contributor
 */
std::future<void> getContributorIssueMetrics(const std::string& username, QueryParams = {} params);

/**
 * Get aggregated review metrics for a contributor
 */
std::future<void> getContributorReviewMetrics(const std::string& username, QueryParams = {} params);

/**
 * Get aggregated comment metrics for a contributor
 */
std::future<void> getContributorCommentMetrics(const std::string& username, QueryParams = {} params);

} // namespace elizaos
