#include ".generateTimeIntervals.hpp"
#include ".getActiveContributors.hpp"
#include ".types.hpp"
#include "aiContributorSummary.hpp"
#include "context.hpp"
#include "mutations.hpp"
#include "queries.hpp"
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

/**
 * Check if a summary already exists for a user on a specific date and interval type
 */
std::future<bool> checkExistingSummary(const std::string& username, string | Date date, IntervalType intervalType););

  return existingSummary !== undefined && existingSummary.summary !== "";
}

/**
 * Generate summaries for all active contributors in a repository for a specific time interval
 */
const generateContributorSummariesForInterval = createStep(
  "ContributorSummaries",
  async (
    {
      interval,
      repoId,
      username,
    }: { interval: TimeInterval; repoId: string | undefined; username: string },
    context: SummarizerPipelineContext,
  ) => {
    const { logger, aiSummaryConfig, overwrite } = context;
    const intervalLogger = logger?.child(interval.intervalType);

    if (!aiSummaryConfig.enabled) {
      return null;
    }

    // Query parameters for this interval
    const dateRange = {
      startDate: toDateString(interval.intervalStart),
      endDate: toDateString(interval.intervalEnd),
    };

    try {
      // Check if summary already exists (skip if overwrite is true)
      if (!overwrite) {
        const summaryExists = await checkExistingSummary(
          username,
          dateRange.startDate,
          interval.intervalType,
        );
        if (summaryExists) {
          intervalLogger?.debug(
            `${interval.intervalType} summary already exists for ${username} on ${dateRange.startDate}, skipping generation`,
          );
          return;
        }
      }

      // Get metrics for this contributor
      const metrics = await getContributorMetrics({
        username,
        dateRange,
      });

      const summary = await generateAISummaryForContributor(
        metrics,
        aiSummaryConfig,
        interval.intervalType,
      );

      if (!summary) {
        intervalLogger?.debug(
          `No activity for ${username} on ${dateRange.startDate}, skipping summary generation`,
        );
        return;
      }
      // Store summary in the database with interval type
      await storeDailySummary(
        username,
        toDateString(interval.intervalStart),
        summary,
        interval.intervalType,
      );

      intervalLogger?.info(
        `Generated and stored ${dateRange.startDate} summary for ${username}`,
        {
          summary,
        },
      );
      return {
        username,
        summary,
      };
    } catch (error) {
      intervalLogger?.error(`Error processing contributor ${username}`, {
        error: (error as Error).message,
      });
    }
  },
);

const generateContributorSummaries = pipe(
  getActiveContributorsInInterval,
  ({ interval, repoId, contributors }) =>
    contributors.map((contributor) => ({
      interval,
      repoId,
      username: contributor.username,
    })),
  mapStep(generateContributorSummariesForInterval),
  (results) => {
    return results.filter(isNotNullOrUndefined);
  },
);
const generateDailyContributorSummaries = pipe(
  generateTimeIntervals<{ repoId: string }>("day"),
  (input, context: SummarizerPipelineContext) => {
    if (context.enabledIntervals.day) {
      return input;
    }
    return [];
  },
  mapStep(generateContributorSummaries),
);
const generateWeeklyContributorSummaries = pipe(
  generateTimeIntervals<{ repoId: string }>("week"),
  (input, context: SummarizerPipelineContext) => {
    if (context.enabledIntervals.week) {
      return input;
    }
    return [];
  },
  mapStep(generateContributorSummaries),
);

const generateMonthlyContributorSummaries = pipe(
  generateTimeIntervals<{ repoId: string }>("month"),
  (input, context: SummarizerPipelineContext) => {
    if (context.enabledIntervals.month) {
      return input;
    }
    return [];
  },
  mapStep(generateContributorSummaries),
);

} // namespace elizaos
