#include ".generateTimeIntervals.hpp"
#include ".getActiveContributors.hpp"
#include ".types.hpp"
#include "calculateTags.hpp"
#include "contributorScores.hpp"
#include "fetchAllContributors.hpp"
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

;
;
;
;
;
;
;

/**
 * Process a single repository for tags
 */
const processContributorTags = pipe(
  // Fetch all unique contributors from all repos
  fetchAllContributors,
  // Process each contributor in parallel
  mapStep(calculateTags),
  // Format the combined results
  createStep("logResults", (results, context) => {
    const totalContributors = results.filter(isNotNullOrUndefined).length;
    context.logger?.info(
      `Processed tags for ${totalContributors} contributors`,
    );
    return results;
  }),
);

const processContributorScores = pipe(
  generateTimeIntervals("day"),
  mapStep(
    pipe(
      // Fetch only active contributors for this interval
      getActiveContributorsInInterval,
      // Process the active contributors for this interval
      processContributorsForInterval,
    ),
  ),
  createStep("logScoringResults", (intervals, context) => {
    const totalIntervals = intervals.length;
    const allUniqueContributors = intervals
      .filter(isNotNullOrUndefined)
      .flatMap((interval) => interval.results.map((r) => r.username));
    const uniqueContributors = [...new Set(allUniqueContributors)];
    context.logger?.info(
      `Processed scores for ${uniqueContributors.length} unique contributors over ${totalIntervals} days`,
    );
    return intervals;
  }),
);

/**
 * Pipeline for calculating all contributor data across repositories
 */
const contributorsPipeline = sequence(
  processContributorTags,
  processContributorScores,
);

} // namespace elizaos
