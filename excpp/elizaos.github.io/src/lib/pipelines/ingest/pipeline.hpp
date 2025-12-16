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

;
;
;
;
;
;
;
;
;
;
;
;
;

/**
 * Process repository data for a specific time interval
 */
const ingestRepoDataForInterval = createStep(
  "ingest",
  async (
    {
      repository,
      interval,
    }: { repository: RepositoryConfig; interval: TimeInterval },
    context: IngestionPipelineContext,
  ) => {
    const { logger, force } = context;
    const { owner, name } = repository;
    const repoId = `${owner}/${name}`;

    const intervalLogger = logger?.child(
      `${interval.intervalType} - ${toDateString(interval.intervalStart)}`,
    );

    // Check if we need to skip this interval based on lastFetched
    if (!force) {
      const repoData = await db.query.repositories.findFirst({
        where: eq(repositories.repoId, repoId),
        columns: {
          lastFetchedAt: true,
        },
      });

      if (repoData?.lastFetchedAt) {
        const lastFetched = new UTCDate(repoData.lastFetchedAt);
        const intervalEnd = new UTCDate(interval.intervalEnd);

        if (lastFetched > intervalEnd) {
          intervalLogger?.info(
            `Skipping interval ${toDateString(interval.intervalStart)} - ${toDateString(interval.intervalEnd)} for ${repoId} as it was already fetched at ${lastFetched.toISOString()}`,
          );
          return {
            repository: repoId,
            prs: 0,
            issues: 0,
            skipped: true,
          };
        }
      }
    }

    // Update dateRange in context if we have an interval
    const processingContext = interval
      ? {
          ...context,
          logger: intervalLogger,
          dateRange: {
            startDate: toDateString(interval.intervalStart),
            endDate: toDateString(interval.intervalEnd),
          },
        }
      : context;

    try {
      // Record the ingestion timestamp
      const ingestionTimestamp = new UTCDate().toISOString();
      logger?.debug(
        `Created ingestion timestamp: ${ingestionTimestamp} for ${repoId}`,
      );

      // Process PRs and issues in parallel
      const [prResult, issueResult] = await parallel(
        () => fetchAndStorePullRequests({ repository }, processingContext),
        () => fetchAndStoreIssues({ repository }, processingContext),
      )(null, processingContext);

      // Update repository last fetched timestamp
      await updateRepositoryLastFetched(repoId, ingestionTimestamp);

      logger?.info(
        `Completed ingestion for ${interval.intervalType} ${repoId}: ${prResult.count} PRs, ${issueResult.count} issues`,
        processingContext.dateRange,
      );

      return {
        repository: repoId,
        prs: prResult.count,
        issues: issueResult.count,
        skipped: false,
      };
    } catch (error) {
      logger?.error(`Error processing repository ${repoId}`, {
        error: String(error),
      });

      return {
        repository: repoId,
        prs: 0,
        issues: 0,
        skipped: false,
      };
    }
  },
);

const ingestWeeklyGithubData = pipe(
  generateTimeIntervals<{ repository: RepositoryConfig }>("week"),
  mapStep(ingestRepoDataForInterval),
  createStep("Filter null results", (results) => {
    return results.filter(isNotNullOrUndefined);
  }),
);

} // namespace elizaos
