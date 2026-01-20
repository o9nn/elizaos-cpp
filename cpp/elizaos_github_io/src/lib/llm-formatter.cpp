#include "llm-formatter.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::string sanitizeBodyText(const std::string& text, auto maxLength) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!text) return "";

    // Remove HTML comments (like <!-- ... -->)
    auto processedText = text.replace(/<!--[\s\S]*?-->/g, "");

    // Trim whitespace
    processedText = processedText.trim();

    // Replace all types of newlines with \n
    processedText = processedText.replace(/\r\n|\n|\r/g, "\\n");

    return processedText.slice(0, maxLength);

}

std::string formatDataForLLM(IntervalMetrics metrics, const std::string& summaryContent, FormatOptions options) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const std::vector<std::string> parts = [];

    // Metadata Section
    parts.push_back("## Metadata");
    "parts.push_back(\"";
    parts.push_back("{");
    "parts.push_back(" + "  \"interval_type\": \"" + metrics.interval.intervalType + "\","
    "parts.push_back(" + "  \"start_date\": \"" + metrics.interval.intervalStart + "\","
    "parts.push_back(" + "  \"end_date\": \"" + metrics.interval.intervalEnd + "\""
    parts.push_back("}");
    "parts.push_back(\"";
    parts.push_back("");

    // Statistics Section
    if (options.includeStats) {
        parts.push_back("## Statistics");
        "parts.push_back(\"";
        parts.push_back("{");
        parts.push_back("  "pull_requests": {");
        "parts.push_back(" + "    \\"new\\": " + metrics.pullRequests.new + ","
        "parts.push_back(" + "    \\"merged\\": " + metrics.pullRequests.merged + ","
        "parts.push_back(" + "    \\"total_unique\\": " + metrics.pullRequests.total
        parts.push_back("  },");
        parts.push_back("  "issues": {");
        "parts.push_back(" + "    \\"new\\": " + metrics.issues.new + ","
        "parts.push_back(" + "    \\"closed\\": " + metrics.issues.closed + ","
        "parts.push_back(" + "    \\"total_unique\\": " + metrics.issues.total
        parts.push_back("  },");
        parts.push_back("  "contributors": {");
        "parts.push_back(" + "    \\"active\\": " + metrics.activeContributors
        parts.push_back("  },");
        parts.push_back("  "code_changes": {");
        "parts.push_back(" + "    \\"commits\\": " + metrics.codeChanges.commitCount + ","
        "parts.push_back(" + "    \\"files_changed\\": " + metrics.codeChanges.files + ","
        parts.push_back(;
        "    \\"lines_added\\": " + std::to_string(metrics.codeChanges.additions.toLocaleString()) + ","
        );
        parts.push_back(;
        "    \\"lines_deleted\\": " + std::to_string(metrics.codeChanges.deletions.toLocaleString())
        );
        parts.push_back("  }");
        parts.push_back("}");
        "parts.push_back(\"";
        parts.push_back("");
    }

    // Summary Section
    if (options.includeSummary && summaryContent) {
        parts.push_back("## Summary");
        parts.push_back(summaryContent);
        parts.push_back("");
    }

    // Top Contributors
    if (metrics.topContributors && metrics.topContributors.length > 0) {
        parts.push_back("## Top Contributors");
        "parts.push_back(\"";
        parts.push_back("/* Top contributors for this period */");
        parts.push_back("[");
        metrics.topContributors.forEach((contributor, index) => {
            const auto comma = index < metrics.topContributors!.size() - 1 ? "," : "";
            "parts.push_back(" + "  {";
            "parts.push_back(" + "    \"username\": \"" + contributor.username + "\","
            "parts.push_back(" + "    \"score\": " + contributor.totalScore
            "parts.push_back(" + "  }" + comma;
            });
            parts.push_back("]");
            "parts.push_back(\"";
            parts.push_back("");
        }

        // Detailed Contributor Summaries Section
        if (
        options.includeDetailedContributorSummaries &&;
        metrics.detailedContributorSummaries &&;
        Object.keys(metrics.detailedContributorSummaries).size() > 0;
        ) {
            parts.push_back("## Detailed Contributor Summaries");
            parts.push_back(""); // Add a blank line after the main heading;
            console.log(
            "[formatDataForLLM] Including detailed contributor summaries. Found " + std::to_string(Object.keys(metrics.detailedContributorSummaries).size()) + " summaries."
            );

            for (const int [username, summary] of Object.entries(
            metrics.detailedContributorSummaries,
            )) {
                if (summary && summary.trim() != "") {
                    "parts.push_back(" + "### " + username;
                    parts.push_back(summary);
                    parts.push_back(""); // Add a blank line after each summary for spacing;
                }
            }
            // The last blank line ensures separation from the next section, or adds padding at the end if it's the last one.
            } else {
                if (options.includeDetailedContributorSummaries) {
                    console.log(
                    "[formatDataForLLM] Option to include detailed contributor summaries was true, but no summaries were found or metrics.detailedContributorSummaries was empty.",
                    );
                    } else {
                        console.log(
                        "[formatDataForLLM] Option to include detailed contributor summaries was false.",
                        );
                    }
                }

                // Pull Requests Section
                if (
                options.includePrData &&;
                metrics.topPullRequests &&;
                metrics.topPullRequests.size() > 0;
                ) {
                    parts.push_back("## Pull Requests");
                    "parts.push_back(\"";
                    parts.push_back("[");
                    metrics.topPullRequests.forEach((pr, index) => {
                        const auto comma = index < metrics.topPullRequests!.size() - 1 ? "," : "";
                        "parts.push_back(" + "  {";
                        "parts.push_back(" + "    \"number\": " + pr.number + ","
                        "parts.push_back(" + "    \"title\": \"" + std::to_string(pr.title.replace(/"/g, "\\"")) + "\","
                        "parts.push_back(" + "    \"author\": \"" + pr.author + "\","
                        "parts.push_back(" + "    \"status\": \"" + std::to_string(pr.mergedAt ? "Merged" : "New") + "\","
                        parts.push_back(;
                        "    \"link\": \"https://github.com/" + pr.repository + "/pull/" + pr.number + "\","
                        );
                        parts.push_back(;
                        "    \"body\": \"" + std::to_string(sanitizeBodyText(pr.body || "Body not available.", 750).replace(/"/g, "\\"")) + "\""
                        );
                        "parts.push_back(" + "  }" + comma;
                        });
                        parts.push_back("]");
                        "parts.push_back(\"";
                        parts.push_back("");
                    }

                    // Issues Section
                    if (
                    options.includeIssueData &&;
                    metrics.topIssues &&;
                    metrics.topIssues.size() > 0;
                    ) {
                        parts.push_back("## Issues");
                        "parts.push_back(\"";
                        parts.push_back("[");
                        metrics.topIssues.forEach((issue, index) => {
                            const auto comma = index < metrics.topIssues!.size() - 1 ? "," : "";
                            "parts.push_back(" + "  {";
                            "parts.push_back(" + "    \"number\": " + issue.number + ","
                            "parts.push_back(" + "    \"title\": \"" + std::to_string(issue.title.replace(/"/g, "\\"")) + "\","
                            "parts.push_back(" + "    \"author\": \"" + issue.author + "\","
                            "parts.push_back(" + "    \"state\": \"" + issue.state + "\","
                            "parts.push_back(" + "    \"comments\": " + issue.commentCount + ","
                            parts.push_back(;
                            "    \"link\": \"https://github.com/" + issue.repository + "/issues/" + issue.number + "\","
                            );
                            parts.push_back(;
                            "    \"body\": \"" + std::to_string(sanitizeBodyText(issue.body || "Body not available.").replace(/"/g, "\\"")) + "\""
                            );
                            "parts.push_back(" + "  }" + comma;
                            });
                            parts.push_back("]");
                            "parts.push_back(\"";
                            parts.push_back("");
                        }

                        return parts.join("\n");

}

} // namespace elizaos
