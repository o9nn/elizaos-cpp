#include ".codeAreaHelpers.hpp"
#include ".export/queries.hpp"
#include "callAIService.hpp"
#include "config.hpp"
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

struct CompletedItem {
    std::string title;
    double prNumber;
    WorkItemType type;
};


struct FocusArea {
    std::string area;
    double count;
};


struct PullRequestMetrics {
    double total;
    double merged;
    double open;
    std::optional<std::vector<unknown>> items;
    std::optional<double> mergedThisPeriod;
};


struct IssueMetrics {
    double total;
    double opened;
    double closed;
    std::optional<std::vector<unknown>> items;
    std::optional<std::vector<unknown>> closedThisPeriod;
};


struct CodeChangeMetrics {
    double additions;
    double deletions;
    double files;
    std::optional<double> commitCount;
};


struct ContributorInfo {
    std::string username;
};


struct ProjectMetricsForSummary {
    PullRequestMetrics pullRequests;
    IssueMetrics issues;
    double uniqueContributors;
    std::vector<ContributorInfo> topContributors;
    CodeChangeMetrics codeChanges;
    std::vector<FocusArea> focusAreas;
    std::vector<CompletedItem> completedItems;
};


std::future<string | null> generateProjectSummary(RepositoryMetrics metrics, AISummaryConfig config, { startDate: string } dateInfo, IntervalType intervalType);

  try {
    // Format the data for the AI prompt based on interval type
    const prompt = formatAnalysisPrompt(
      metrics,
      dateInfo,
      intervalType,
      config,
    );

    // Calculate token length based on prompt content and interval type
    const maxTokens = calculateMaxTokens(prompt, intervalType, config);
    console.log(`Max tokens: ${maxTokens}, intervalType: ${intervalType}`);
    // Get analysis from AI model
    return await callAIService(prompt, config, {
      maxTokens,
      model: config.models[intervalType],
    });
  } catch (error) {
    console.error(`Error generating ${intervalType} project analysis:`, error);
    return null;
  }
}

/**
 * Calculate appropriate max tokens based on prompt length and interval type
 * Returns a value using a basic scaling approach
 */
;

  // Get base token count for this interval type
  const baseTokens = baseTokensByInterval[intervalType] || 600;

  // Simple estimation: 1 token ≈ 4 characters in English
  const estimatedPromptTokens = prompt.length / 4;
  // Add 20% more tokens for every 400 estimated tokens in the prompt
  const scalingFactor = 1 + Math.floor(estimatedPromptTokens / 400) * 0.2;

  // Calculate final token count
  const calculatedTokens = Math.round(baseTokens * scalingFactor);

  // Ensure result is within a reasonable range, respecting config.max_tokens
  return Math.max(300, Math.min(config.max_tokens, calculatedTokens));
}

/**
 * Format project metrics into a structured prompt for analysis based on interval type
 */
,
  intervalType: IntervalType,
  config: AISummaryConfig,
): string {
  const date = new UTCDate(dateInfo.startDate);

  // Format date information based on interval type
  const timeframeTitle = formatTimeframeTitle(date, intervalType);

  // Format top active areas
  const topActiveAreas = metrics.focusAreas
    .sort((a, b) => b.count - a.count)
    .slice(0, 5)
    .map((area) => `${area.area}: ${area.count} changes`);

  const formatCompletedItems = (type: WorkItemType) =>
    metrics.completedItems
      .filter((item) => item.type === type)
      .map(
        (item) => ` (PR #${item.prNumber}) ${item.title}. BODY: ${item.body}`,
      )
      .join("\n- ") || "None";

  // Format completed items for better clarity
  const completedFeatures = formatCompletedItems("feature");
  const completedBugfixes = formatCompletedItems("bugfix");
  const completedRefactors = formatCompletedItems("refactor");
  const completedDocs = formatCompletedItems("docs");
  const completedTests = formatCompletedItems("tests");
  const completedOtherWork = formatCompletedItems("other");
  const newIssues = metrics.issues.newIssues;
  const closedIssues = metrics.issues.closedIssues;

  return `
BACKGROUND CONTEXT:
  ${config.projectContext}

INSTRUCTIONS:
Generate a detailed yet concise ${intervalType}ly development report for the ${metrics.repository} repo during ${timeframeTitle}, based on the following github activity.
  
COMPLETED WORK:
  
- **Features Added:** 
  - ${completedFeatures}
- **Bug Fixes:** 
  - ${completedBugfixes}
- **Code Refactoring:** 
  - ${completedRefactors}
- **Documentation Enhancements:** 
  - ${completedDocs}
- **Tests Added:** 
  - ${completedTests}
- **Other Work:** 
  - ${completedOtherWork}
  Most Active Development Areas:
  - ${topActiveAreas.join("\n- ")}

NEW ISSUES:
  - ${newIssues.map((issue) => `[#${issue.number}] ${issue.title}. BODY: ${issue.body?.slice(0, 240)}`).join("\n- ")}

CLOSED ISSUES:
  - ${closedIssues.map((issue) => `[#${issue.number}] ${issue.title}. BODY: ${issue.body?.slice(0, 240)}`).join("\n- ")}

Format the report with the following sections:

# <Project Name> ${getIntervalTypeTitle(intervalType)} Update (${timeframeTitle})
## OVERVIEW 
  Provide a high-level summary (max 500 characters, min 40 characters) highlighting the overall progress and major achievements of the ${intervalType}.

## KEY TECHNICAL DEVELOPMENTS

  Group/cluster the completed work thematically into ${intervalType === "month" ? "8-12" : "2-4"} different headlines,
  and concisely describe the key changes and improvements in point form. Reference
   the PR numbers that are most relevant to each headline, formatted as a Markdown link (e.g. [#123](https://github.com/${metrics.repository}/pull/123)).
 
## CLOSED ISSUES

  Group related closed issues into  ${intervalType === "month" ? "6-9" : "2-4"} different headlines and concisely summarize them.
   Reference the issue numbers that are most relevant to each headline, formatted as a Markdown link (e.g. [#123](https://github.com/${metrics.repository}/issues/123)).

## NEW ISSUES

  Group the new issues thematically into ${intervalType === "month" ? "6-9" : "2-4"} different headlines,
  and concisely describe the key challenges and problems in point form. Reference
  the issue numbers that are most relevant to each headline, formatted as a Markdown link (e.g. [#123](https://github.com/${metrics.repository}/issues/123)).

 ${
   intervalType === "month"
     ? `
## SUMMARY
Close with a short summary of the months achievements
`
     : ""
 }

GUIDELINES:
- Be factual and precise; focus on concrete changes and verifiable data.
- Use clear, accessible language for both technical and non-technical audiences.
- Ensure all information is organized into the specified sections for clarity.
- Use markdown formatting for the report.
`;
}

} // namespace elizaos
