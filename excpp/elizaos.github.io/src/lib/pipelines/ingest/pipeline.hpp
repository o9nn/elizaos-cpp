#include ".generateTimeIntervals.hpp"
#include ".types.hpp"
#include "context.hpp"
#include "mutations.hpp"
#include "storeIssues.hpp"
#include "storePullRequests.hpp"
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
 * Process repository data for a specific time interval
 */

    // Check if we need to skip this interval based on lastFetched

    // Update dateRange in context if we have an interval

      // Record the ingestion timestamp

      // Process PRs and issues in parallel

      // Update repository last fetched timestamp
      await updateRepositoryLastFetched(repoId, ingestionTimestamp);


} // namespace elizaos
