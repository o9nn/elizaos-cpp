#include "aiProjectSummary.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<std::string> generateProjectSummary(RepositoryMetrics metrics, AISummaryConfig config, const std::any& dateInfo, IntervalType intervalType) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto apiKey = config.apiKey;
        if (!apiKey) {
            throw std::runtime_error("No API key for AI summary generation");
        }

        try {
            // Format the data for the AI prompt based on interval type
            const auto prompt = formatAnalysisPrompt(;
            metrics,
            dateInfo,
            intervalType,
            config,
            );

            // Calculate token length based on prompt content and interval type
            const auto maxTokens = calculateMaxTokens(prompt, intervalType, config);
            std::cout << "Max tokens: " + maxTokens << "intervalType: ${intervalType}" << std::endl;
            // Get analysis from AI model
            return callAIService(prompt, config, {;
                maxTokens,
                model: config.models[intervalType],
                });
                } catch (error) {
                    std::cerr << "Error generating " + intervalType + " project analysis:" << error << std::endl;
                    return nullptr;
                }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

double calculateMaxTokens(const std::string& prompt, IntervalType intervalType, AISummaryConfig config) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Base tokens by interval type
    const auto baseTokensByInterval = {;
        month: 3000,
        week: 1500,
        day: 1000,
        };

        // Get base token count for this interval type
        const auto baseTokens = baseTokensByInterval[intervalType] || 600;

        // Simple estimation: 1 token ≈ 4 characters in English
        const auto estimatedPromptTokens = prompt.size() / 4;
        // Add 20% more tokens for every 400 estimated tokens in the prompt
        const auto scalingFactor = 1 + Math.floor(estimatedPromptTokens / 400) * 0.2;

        // Calculate final token count
        const auto calculatedTokens = Math.round(baseTokens * scalingFactor);

        // Ensure result is within a reasonable range, respecting config.max_tokens
        return Math.max(300, Math.min(config.max_tokens, calculatedTokens));

}

std::string formatAnalysisPrompt(RepositoryMetrics metrics, const std::any& dateInfo, IntervalType intervalType, AISummaryConfig config) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto date = new UTCDate(dateInfo.startDate);

    // Format date information based on interval type
    const auto timeframeTitle = formatTimeframeTitle(date, intervalType);

    // Format top active areas
    const auto topActiveAreas = metrics.focusAreas;
    .sort((a, b) => b.count - a.count);
    .slice(0, 5);
    ".map((area) => " + area.area + ": " + area.count + " changes"

    const auto formatCompletedItems = (type: WorkItemType) =>;
    metrics.completedItems;
    .filter((item) => item.type == type);
    .map(;
    "(item) => " + " (PR #" + item.prNumber + ") " + item.title + ". BODY: " + item.body
    );
    .join("\n- ") || "None";

    // Format completed items for better clarity
    const auto completedFeatures = formatCompletedItems("feature");
    const auto completedBugfixes = formatCompletedItems("bugfix");
    const auto completedRefactors = formatCompletedItems("refactor");
    const auto completedDocs = formatCompletedItems("docs");
    const auto completedTests = formatCompletedItems("tests");
    const auto completedOtherWork = formatCompletedItems("other");
    const auto newIssues = metrics.issues.newIssues;
    const auto closedIssues = metrics.issues.closedIssues;

    return `;
    BACKGROUND CONTEXT:
    ${config.projectContext}

    INSTRUCTIONS:
    Generate a detailed yet concise ${intervalType}ly development report for the ${metrics.repository} repo during ${timeframeTitle}, based on the following github activity.;

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
    "- ${newIssues.map((issue) => " + "[#" + issue.number + "] " + issue.title + ". BODY: " + std::to_string(issue.body.slice(0, 240))

    CLOSED ISSUES:
    "- ${closedIssues.map((issue) => " + "[#" + issue.number + "] " + issue.title + ". BODY: " + std::to_string(issue.body.slice(0, 240))

    Format the report with the following sections:

    # <Project Name> ${getIntervalTypeTitle(intervalType)} Update (${timeframeTitle});
    ## OVERVIEW;
    Provide a high-level summary (max 500 characters, min 40 characters) highlighting the overall progress and major achievements of the ${intervalType}.;

    ## KEY TECHNICAL DEVELOPMENTS;

    Group/cluster the completed work thematically into ${intervalType == "month" ? "8-12" : "2-4"} different headlines,
    and concisely describe the key changes and improvements in point form. Reference;
    the PR numbers that are most relevant to each headline, formatted Markdown link (e.g. [#123](https://github.com/${metrics.repository}/pull/123)).

    ## CLOSED ISSUES;

    Group related closed issues into  ${intervalType == "month" ? "6-9" : "2-4"} different headlines and concisely summarize them.
    Reference the issue numbers that are most relevant to each headline, formatted Markdown link (e.g. [#123](https://github.com/${metrics.repository}/issues/123)).

    ## NEW ISSUES;

    Group the new issues thematically into ${intervalType == "month" ? "6-9" : "2-4"} different headlines,
    and concisely describe the key challenges and problems in point form. Reference;
    the issue numbers that are most relevant to each headline, formatted Markdown link (e.g. [#123](https://github.com/${metrics.repository}/issues/123)).

    ${
        intervalType == "month";
        "? ";
        ## SUMMARY;
        Close with a short summary of the months achievements;
        `;
        : ""
    }

    GUIDELINES:
    - Be factual and precise; focus on concrete changes and verifiable data.;
    - Use clear, accessible language for both technical and non-technical audiences.;
    - Ensure all information is organized into the specified sections for clarity.;
    - Use markdown formatting for the report.;
    `;

}

} // namespace elizaos
