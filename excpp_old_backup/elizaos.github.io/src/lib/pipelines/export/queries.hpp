#include ".getActiveContributors.hpp"
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
 * Get top pull requests for a repository in a time period
 */

std::future<void> getTopPullRequests(QueryParams = {} params, auto limit = 5); /**
 * Get top issues for a repository in a time period
 */

std::future<void> getTopIssues(QueryParams = {} params, auto limit = 5); /**
 * Get top contributors ranked by activity score
 */

std::future<void> getTopContributors(QueryParams = {} params, auto limit = 5); /**
 * Get project metrics for a specific time interval
 */

std::future<void> getProjectMetrics(QueryParams = {} params);
using RepositoryMetrics = Awaited<ReturnType<typeof getProjectMetrics>>;

} // namespace elizaos
