#include "callAIService.hpp"
#include "config.hpp"
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

using ContributorMetricsForSummary = Awaited<
  ReturnType<typeof getContributorMetrics>
>;

/**
 * Generate an AI summary of a contributor's activity
 */
std::future<string | null> generateAISummaryForContributor(ContributorMetricsForSummary metrics, AISummaryConfig config, IntervalType intervalType);

  // Skip summary generation if no meaningful activity
  const hasActivity =
    metrics.pullRequests.merged > 0 ||
    metrics.pullRequests.open > 0 ||
    metrics.issues.total > 0 ||
    metrics.reviews.total > 0 ||
    // metrics.comments.total > 0 ||
    metrics.codeChanges.files > 0;

  if (!hasActivity) {
    return null;
  }

  try {
    // Format the metrics data for the AI prompt
    const prompt = formatContributorPrompt(metrics, intervalType);

    // Get summary from AI model
    return await callAIService(prompt, config, {
      model: config.models[intervalType],
    });
  } catch (error) {
    console.error(`Error generating summary for ${metrics.username}:`, error);
    return null;
  }
}

/**
 * Format contributor metrics into a structured prompt
 */
;

  // Get time period description for the prompt
  const timePeriod = getTimePeriodText(intervalType);

  // Get the most significant directories from focus areas
  const topDirs = metrics.focusAreas
    .sort((a, b) => b.count - a.count)
    .slice(0, 3)
    .map((area) => {
      const parts = area.area.split("/");
      // If it's a package, use the package name
      if (parts.includes("packages")) {
        const pkgIndex = parts.indexOf("packages");
        return parts[pkgIndex + 1] || area.area;
      }
      // For docs, distinguish between package and markdown files
      if (parts[0] === "docs" || parts.includes("docs")) {
        return "docs-package";
      }
      if (
        area.area.endsWith(".md") ||
        area.area.endsWith(".mdx") ||
        area.area.includes("/docs/") ||
        area.area.includes("documentation")
      ) {
        return "documentation";
      }
      // Otherwise use the first meaningful directory
      return parts[0] || area.area;
    });

  // Format merged PRs
  const mergedPRDetails = metrics.pullRequests.items
    .filter((pr) => pr.merged === 1)
    .map((pr) => {
      const additions =
        pr.commits?.reduce((sum, c) => sum + (c.additions || 0), 0) || 0;
      const deletions =
        pr.commits?.reduce((sum, c) => sum + (c.deletions || 0), 0) || 0;

      return `${pr.repository}#${pr.number} "${truncateTitle(
        pr.title,
      )}" (+${additions}/-${deletions} lines)`;
    })
    .join(", ");

  // Format open PRs
  const openPRDetails = metrics.pullRequests.items
    .filter((pr) => pr.merged !== 1)
    .map((pr) => `${pr.repository}#${pr.number} "${truncateTitle(pr.title)}"`)
    .join(", ");

  // Format issues
  const issueDetails = metrics.issues.items
    .map(
      (issue) =>
        `${issue.repository}#${issue.number} "${truncateTitle(issue.title)}" (${
          issue.state
        })`,
    )
    .join(", ");

  // Work pattern analysis
  const workPatternDescription =
    metrics.activityPattern.frequency >= 0.7
      ? "very consistent work (active most days)"
      : metrics.activityPattern.frequency >= 0.4
        ? "moderately consistent work (active several days per week)"
        : metrics.activityPattern.frequency >= 0.2
          ? "occasional activity (active a few days per week)"
          : "sporadic activity (active a few days this period)";

  // Commit type analysis
  const commitTypes = metrics.codeChanges.commitTypes;
  const totalCommits = metrics.codeChanges.commitCount;

  let workFocus = "";
  if (totalCommits > 0) {
    const typePercentages = {
      feature: Math.round((commitTypes.feature / totalCommits) * 100),
      bugfix: Math.round((commitTypes.bugfix / totalCommits) * 100),
      refactor: Math.round((commitTypes.refactor / totalCommits) * 100),
      docs: Math.round((commitTypes.docs / totalCommits) * 100),
      tests: Math.round((commitTypes.tests / totalCommits) * 100),
      other: Math.round((commitTypes.other / totalCommits) * 100),
    };

    const sortedTypes = Object.entries(typePercentages)
      .filter(([_, percentage]) => percentage > 10)
      .sort(([_, a], [_2, b]) => b - a);

    if (sortedTypes.length > 0) {
      workFocus = sortedTypes
        .map(([type, percentage]) => `${type} work (${percentage}%)`)
        .join(", ");
    }
  }

  // File type analysis
  const fileTypes = metrics.pullRequests.fileTypes;
  const totalFiles = Object.values(fileTypes).reduce(
    (sum, count) => sum + count,
    0,
  );

  let fileTypesFocus = "";
  if (totalFiles > 0) {
    const filePercentages = {
      code: Math.round((fileTypes.code / totalFiles) * 100),
      tests: Math.round((fileTypes.tests / totalFiles) * 100),
      docs: Math.round((fileTypes.docs / totalFiles) * 100),
      config: Math.round((fileTypes.config / totalFiles) * 100),
    };

    const sortedFileTypes = Object.entries(filePercentages)
      .filter(([_, percentage]) => percentage > 10)
      .sort(([_, a], [__, b]) => b - a);

    if (sortedFileTypes.length > 0) {
      fileTypesFocus = sortedFileTypes
        .map(([type, percentage]) => `${type} (${percentage}%)`)
        .join(", ");
    }
  }

  // PR complexity insights
  const prMetrics = metrics.pullRequests.metrics;
  const prComplexityInsights =
    metrics.pullRequests.merged > 0
      ? `Average PR: +${prMetrics.avgAdditions}/-${prMetrics.avgDeletions} lines, ${prMetrics.avgTimeToMerge} hours to merge
Largest PR: ${prMetrics.largestPR.repository}#${prMetrics.largestPR.number} with +${prMetrics.largestPR.additions}/-${prMetrics.largestPR.deletions} lines`
      : "No merged PRs";

  // Build the summary prompt
  return `Summarize ${metrics.username}'s contributions ${timePeriod.timeFrame}:

PULL REQUESTS:
- Merged: ${
    metrics.pullRequests.merged > 0
      ? `${metrics.pullRequests.merged} PRs: ${mergedPRDetails}`
      : "None"
  }
- Open: ${
    metrics.pullRequests.open > 0
      ? `${metrics.pullRequests.open} PRs: ${openPRDetails}`
      : "None"
  }
- Complexity: ${prComplexityInsights}

ISSUES:
- Created: ${metrics.issues.opened > 0 ? metrics.issues.opened : "None"} ${
    issueDetails ? `(${issueDetails})` : ""
  }
- Closed: ${metrics.issues.closed > 0 ? metrics.issues.closed : "None"}
- Commented on: ${
    metrics.issues.commented > 0 ? metrics.issues.commented : "None"
  }

REVIEWS & COMMENTS:
- Reviews: ${
    metrics.reviews.total > 0
      ? `${metrics.reviews.total} total (${metrics.reviews.approved} approvals, ${metrics.reviews.changesRequested} change requests, ${metrics.reviews.commented} comments)`
      : "None"
  }
- PR Comments: ${
    metrics.comments.prComments > 0 ? metrics.comments.prComments : "None"
  }
- Issue Comments: ${
    metrics.comments.issueComments > 0 ? metrics.comments.issueComments : "None"
  }

CODE CHANGES:
${
  metrics.codeChanges.files > 0
    ? `- Modified ${metrics.codeChanges.files} files (+${
        metrics.codeChanges.additions
      }/-${metrics.codeChanges.deletions} lines)
- Commits: ${metrics.codeChanges.commitCount}
- Primary focus: ${workFocus || "Mixed work"}
- File types: ${fileTypesFocus || "Various file types"}`
    : "No code changes"
}

ACTIVITY PATTERN:
- Active on ${metrics.activityPattern.daysActive} out of ${
    metrics.activityPattern.totalDays
  } days
- Pattern: ${workPatternDescription}

PRIMARY AREAS: ${topDirs.join(", ") || "N/A"}

Write a natural, factual summary that:
1. Starts with "${metrics.username}: "
2. Highlights the most significant contributions based on the data
3. Emphasizes meaningful patterns (e.g., "focused on bug fixes in the UI", "major refactoring effort")
4. Includes line counts for significant code changes
5. Groups similar activities in the same repository together (e.g., "merged 3 PRs improving the frontend in elizaos/eliza")
6. Uses repository names + PR/issue numbers when referring to specific contributions (e.g., "elizaos/eliza#123", "elizaos-plugins/plugin-A#45")
7. Omits any activity type that shows "None" above
8. Uses at most ${timePeriod.sentenceCount} sentences
9. Varies sentence structure based on the actual work done

Example good summaries:
"No activity ${timePeriod.timeFrameShort}."
"Focused on UI improvements with 3 merged PRs (+2k/-500 lines), consistently active with daily commits."
"Fixed 4 critical bugs in the authentication system (PRs #123, #124) and reviewed 7 PRs, primarily working on backend code."
"Led documentation efforts with substantial contributions to the API docs (+1.2k lines), opened 3 issues for missing features, and provided 5 detailed code reviews."`;
}

} // namespace elizaos
