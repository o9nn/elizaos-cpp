#include ".getSelectedRepositories.hpp"
#include ".types.hpp"
#include "context.hpp"
#include "generateContributorSummary.hpp"
#include "generateProjectSummary.hpp"
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
 * Pipeline for generating AI-powered summaries of contributor activity
 */
;
;
;
;
;

const generateContributorSummariesForRepo = pipe(
  sequence(
    generateMonthlyContributorSummaries,
    generateWeeklyContributorSummaries,
    generateDailyContributorSummaries,
  ),
  ([weeklyResults, monthlyResults, dailyResults], context) => {
    const numWeeklySummaries = weeklyResults.reduce((acc, result) => {
      return acc + result.length;
    }, 0);
    const numMonthlySummaries = monthlyResults.reduce((acc, result) => {
      return acc + result.length;
    }, 0);
    const numDailySummaries = dailyResults.reduce((acc, result) => {
      return acc + result.length;
    }, 0);
    context.logger?.info(
      `Generated ${numWeeklySummaries} weekly summaries over ${weeklyResults.length} weeks`,
    );
    context.logger?.info(
      `Generated ${numMonthlySummaries} monthly summaries over ${monthlyResults.length} months`,
    );
    context.logger?.info(
      `Generated ${numDailySummaries} daily summaries over ${dailyResults.length} days`,
    );
  },
);

{ type SummarizerPipelineContext, createSummarizerContext };

const contributorSummariesPipeline = pipe(
  getSelectedRepositories,
  mapStep(generateContributorSummariesForRepo),
);

// Pipeline for generating monthly project summaries
const projectSummariesPipeline = pipe(
  getSelectedRepositories,
  mapStep(
    sequence(
      generateDailyProjectSummaries,
      generateWeeklyProjectSummaries,
      generateMonthlyProjectSummaries,
    ),
  ),
  createStep("Log Project Summaries", (results, context) => {
    for (const repo of results) {
      const [daily, weekly, monthly] = repo;
      context.logger?.info(`Generated ${daily.length} daily summaries`);
      context.logger?.info(`Generated ${weekly.length} weekly summaries`);
      context.logger?.info(`Generated ${monthly.length} monthly summaries`);
    }
  }),
);

} // namespace elizaos
