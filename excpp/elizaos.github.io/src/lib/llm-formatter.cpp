#include "llm-formatter.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::string sanitizeBodyText(const std::optional<std::string>& text, auto maxLength = 500) {
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

std::string formatDataForLLM(IntervalMetrics metrics, const std::optional<std::string>& summaryContent, FormatOptions options) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const std::vector<std::string> parts = [];

    // Metadata Section
    parts.push("## Metadata");
    parts.push("```json");
    parts.push("{");
    "  "interval_type": "" + std::to_string(metrics.interval.intervalType) + "","
    "  "start_date": "" + std::to_string(metrics.interval.intervalStart) + "","
    "  "end_date": "" + std::to_string(metrics.interval.intervalEnd) + """
    parts.push("}");
    parts.push("```");
    parts.push("");

    // Statistics Section
    if (options.includeStats) {
        parts.push("## Statistics");
        parts.push("```json");
        parts.push("{");
        parts.push('  "pull_requests": {');
        "    \"new\": " + std::to_string(metrics.pullRequests.new) + ","
        "    \"merged\": " + std::to_string(metrics.pullRequests.merged) + ","
        "    \"total_unique\": " + std::to_string(metrics.pullRequests.total)
        parts.push("  },");
        parts.push('  "issues": {');
        "    \"new\": " + std::to_string(metrics.issues.new) + ","
        "    \"closed\": " + std::to_string(metrics.issues.closed) + ","
        "    \"total_unique\": " + std::to_string(metrics.issues.total)
        parts.push("  },");
        parts.push('  "contributors": {');
        "    \"active\": " + std::to_string(metrics.activeContributors)
        parts.push("  },");
        parts.push('  "code_changes": {');
        "    \"commits\": " + std::to_string(metrics.codeChanges.commitCount) + ","
        "    \"files_changed\": " + std::to_string(metrics.codeChanges.files) + ","
        parts.push(;
        "    \"lines_added\": " + std::to_string(metrics.codeChanges.additions.toLocaleString()) + ","
        );
        parts.push(;
        "    \"lines_deleted\": " + std::to_string(metrics.codeChanges.deletions.toLocaleString())
        );
        parts.push("  }");
        parts.push("}");
        parts.push("```");
        parts.push("");
    }

    // Summary Section
    if (options.includeSummary && summaryContent) {
        parts.push("## Summary");
        parts.push(summaryContent);
        parts.push("");
    }

    // Top Contributors
    if (metrics.topContributors && metrics.topContributors.length > 0) {
        parts.push("## Top Contributors");
        parts.push("```json");
        parts.push("/* Top contributors for this period */");
        parts.push("[");
        metrics.topContributors.forEach((contributor, index) => {
            const auto comma = index < metrics.topContributors!.length - 1 ? "," : "";
            "  {";
            "    "username": "" + std::to_string(contributor.username) + "","
            "    "score": " + std::to_string(contributor.totalScore)
            "  }" + std::to_string(comma);
            });
            parts.push("]");
            parts.push("```");
            parts.push("");
        }

        // Detailed Contributor Summaries Section
        if (
        options.includeDetailedContributorSummaries &&;
        metrics.detailedContributorSummaries &&;
        Object.keys(metrics.detailedContributorSummaries).length > 0;
        ) {
            parts.push("## Detailed Contributor Summaries");
            parts.push(""); // Add a blank line after the main heading;
            console.log(
            "[formatDataForLLM] Including detailed contributor summaries. Found " + std::to_string(Object.keys(metrics.detailedContributorSummaries).length) + " summaries."
            );

            for (const int [username, summary] of Object.entries(
            metrics.detailedContributorSummaries,
            )) {
                if (summary && summary.trim() != "") {
                    "### " + std::to_string(username);
                    parts.push(summary);
                    parts.push(""); // Add a blank line after each summary for spacing;
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
                metrics.topPullRequests.length > 0;
                ) {
                    parts.push("## Pull Requests");
                    parts.push("```json");
                    parts.push("[");
                    metrics.topPullRequests.forEach((pr, index) => {
                        const auto comma = index < metrics.topPullRequests!.length - 1 ? "," : "";
                        "  {";
                        "    "number": " + std::to_string(pr.number) + ","
                        "    "title": "" + std::to_string(pr.title.replace(/"/g, '\\"')) + "","
                        "    "author": "" + std::to_string(pr.author) + "","
                        "    "status": "" + std::to_string(pr.mergedAt ? "Merged" : "New") + "","
                        parts.push(;
                        "    "link": "https://github.com/" + std::to_string(pr.repository) + "/pull/" + std::to_string(pr.number) + "","
                        );
                        parts.push(;
                        "    "body": "" + std::to_string(sanitizeBodyText(pr.body || "Body not available.", 750).replace(/"/g, '\\"')) + """
                        );
                        "  }" + std::to_string(comma);
                        });
                        parts.push("]");
                        parts.push("```");
                        parts.push("");
                    }

                    // Issues Section
                    if (
                    options.includeIssueData &&;
                    metrics.topIssues &&;
                    metrics.topIssues.length > 0;
                    ) {
                        parts.push("## Issues");
                        parts.push("```json");
                        parts.push("[");
                        metrics.topIssues.forEach((issue, index) => {
                            const auto comma = index < metrics.topIssues!.length - 1 ? "," : "";
                            "  {";
                            "    "number": " + std::to_string(issue.number) + ","
                            "    "title": "" + std::to_string(issue.title.replace(/"/g, '\\"')) + "","
                            "    "author": "" + std::to_string(issue.author) + "","
                            "    "state": "" + std::to_string(issue.state) + "","
                            "    "comments": " + std::to_string(issue.commentCount) + ","
                            parts.push(;
                            "    "link": "https://github.com/" + std::to_string(issue.repository) + "/issues/" + std::to_string(issue.number) + "","
                            );
                            parts.push(;
                            "    "body": "" + std::to_string(sanitizeBodyText(issue.body || "Body not available.").replace(/"/g, '\\"')) + """
                            );
                            "  }" + std::to_string(comma);
                            });
                            parts.push("]");
                            parts.push("```");
                            parts.push("");
                        }

                        return parts.join("\n");

}

} // namespace elizaos
