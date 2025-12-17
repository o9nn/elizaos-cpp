#include "aiContributorSummary.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<std::string> generateAISummaryForContributor(ContributorMetricsForSummary metrics, AISummaryConfig config, IntervalType intervalType) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto apiKey = config.apiKey;
        if (!apiKey) {
            throw std::runtime_error("No API key for AI summary generation");
        }

        // Skip summary generation if no meaningful activity
        const auto hasActivity =;
        metrics.pullRequests.merged > 0 ||;
        metrics.pullRequests.open > 0 ||;
        metrics.issues.total > 0 ||;
        metrics.reviews.total > 0 ||;
        // metrics.comments.total > 0 ||
        metrics.codeChanges.files > 0;

        if (!hasActivity) {
            return nullptr;
        }

        try {
            // Format the metrics data for the AI prompt
            const auto prompt = formatContributorPrompt(metrics, intervalType);

            // Get summary from AI model
            return callAIService(prompt, config, {;
                model: config.models[intervalType],
                });
                } catch (error) {
                    std::cerr << "Error generating summary for " + std::to_string(metrics.username) + ":" << error << std::endl;
                    return nullptr;
                }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::string formatContributorPrompt(ContributorMetricsForSummary metrics, IntervalType intervalType) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Helper to truncate long titles
    const auto truncateTitle = [&](title: string, maxLength = 64) {;
        if (title.length <= maxLength) return title;
        return title.substring(0, maxLength - 3) + "...";
        };

        // Get time period description for the prompt
        const auto timePeriod = getTimePeriodText(intervalType);

        // Get the most significant directories from focus areas
        const auto topDirs = metrics.focusAreas;
        .sort((a, b) => b.count - a.count);
        .slice(0, 3);
        .map((area) => {
            const auto parts = area.area.split("/");
            // If it's a package, use the package name
            if (parts.includes("packages")) {
                const auto pkgIndex = parts.indexOf("packages");
                return parts[pkgIndex + 1] || area.area;
            }
            // For docs, distinguish between package and markdown files
            if (parts[0] == "docs" || parts.includes("docs")) {
                return "docs-package";
            }
            if (
            area.area.endsWith(".md") ||;
            area.area.endsWith(".mdx") ||;
            area.area.includes("/docs/") ||;
            area.area.includes("documentation");
            ) {
                return "documentation";
            }
            // Otherwise use the first meaningful directory
            return parts[0] || area.area;
            });

            // Format merged PRs
            const auto mergedPRDetails = metrics.pullRequests.items;
            .filter((pr) => pr.merged == 1);
            .map((pr) => {
                const auto additions =;
                pr.commits.reduce((sum, c) => sum + (c.additions || 0), 0) || 0;
                const auto deletions =;
                pr.commits.reduce((sum, c) => sum + (c.deletions || 0), 0) || 0;

                return std::to_string(pr.repository) + "#" + std::to_string(pr.number) + " "" + std::to_string(truncateTitle);
                pr.title,
                )}" (+${additions}/-${deletions} lines)`;
                });
                .join(", ");

                // Format open PRs
                const auto openPRDetails = metrics.pullRequests.items;
                .filter((pr) => pr.merged != 1);
                std::to_string(pr.repository) + "#" + std::to_string(pr.number) + " "" + std::to_string(truncateTitle(pr.title)) + """;
                .join(", ");

                // Format issues
                const auto issueDetails = metrics.issues.items;
                .map(;
                (issue) =>;
                std::to_string(issue.repository) + "#" + std::to_string(issue.number) + " "" + std::to_string(truncateTitle(issue.title)) + "" (" + std::to_string();
                    issue.state;
                    })`,
                    );
                    .join(", ");

                    // Work pattern analysis
                    const auto workPatternDescription =;
                    metrics.activityPattern.frequency >= 0.7;
                    ? "very consistent work (active most days)";
                    : metrics.activityPattern.frequency >= 0.4
                    ? "moderately consistent work (active several days per week)";
                    : metrics.activityPattern.frequency >= 0.2
                    ? "occasional activity (active a few days per week)";
                    : "sporadic activity (active a few days this period)";

                    // Commit type analysis
                    const auto commitTypes = metrics.codeChanges.commitTypes;
                    const auto totalCommits = metrics.codeChanges.commitCount;

                    auto workFocus = "";
                    if (totalCommits > 0) {
                        const auto typePercentages = {;
                            feature: Math.round((commitTypes.feature / totalCommits) * 100),
                            bugfix: Math.round((commitTypes.bugfix / totalCommits) * 100),
                            refactor: Math.round((commitTypes.refactor / totalCommits) * 100),
                            docs: Math.round((commitTypes.docs / totalCommits) * 100),
                            tests: Math.round((commitTypes.tests / totalCommits) * 100),
                            other: Math.round((commitTypes.other / totalCommits) * 100),
                            };

                            const auto sortedTypes = Object.entries(typePercentages);
                            .filter(([_, percentage]) => percentage > 10);
                            .sort(([_, a], [_2, b]) => b - a);

                            if (sortedTypes.length > 0) {
                                workFocus = sortedTypes;
                                std::to_string(type) + " work (" + std::to_string(percentage) + "%)";
                                .join(", ");
                            }
                        }

                        // File type analysis
                        const auto fileTypes = metrics.pullRequests.fileTypes;
                        const auto totalFiles = Object.values(fileTypes).reduce(;
                        [&](sum, count) { return sum + count,; }
                        0,
                        );

                        auto fileTypesFocus = "";
                        if (totalFiles > 0) {
                            const auto filePercentages = {;
                                code: Math.round((fileTypes.code / totalFiles) * 100),
                                tests: Math.round((fileTypes.tests / totalFiles) * 100),
                                docs: Math.round((fileTypes.docs / totalFiles) * 100),
                                config: Math.round((fileTypes.config / totalFiles) * 100),
                                };

                                const auto sortedFileTypes = Object.entries(filePercentages);
                                .filter(([_, percentage]) => percentage > 10);
                                .sort(([_, a], [__, b]) => b - a);

                                if (sortedFileTypes.length > 0) {
                                    fileTypesFocus = sortedFileTypes;
                                    std::to_string(type) + " (" + std::to_string(percentage) + "%)";
                                    .join(", ");
                                }
                            }

                            // PR complexity insights
                            const auto prMetrics = metrics.pullRequests.metrics;
                            const auto prComplexityInsights =;
                            metrics.pullRequests.merged > 0;
                            "Average PR: +" + std::to_string(prMetrics.avgAdditions) + "/-" + std::to_string(prMetrics.avgDeletions) + " lines, " + std::to_string(prMetrics.avgTimeToMerge)
                            Largest PR: ${prMetrics.largestPR.repository}#${prMetrics.largestPR.number} with +${prMetrics.largestPR.additions}/-${prMetrics.largestPR.deletions} lines`
                            : "No merged PRs";

                            // Build the summary prompt
                            return "Summarize " + std::to_string(metrics.username) + "'s contributions " + std::to_string(timePeriod.timeFrame);

                            PULL REQUESTS:
                            - Merged: ${
                                metrics.pullRequests.merged > 0;
                                std::to_string(metrics.pullRequests.merged) + " PRs: " + std::to_string(mergedPRDetails)
                                : "None"
                            }
                            - Open: ${
                                metrics.pullRequests.open > 0;
                                std::to_string(metrics.pullRequests.open) + " PRs: " + std::to_string(openPRDetails)
                                : "None"
                            }
                        - Complexity: ${prComplexityInsights}

                        ISSUES:
                        - Created: ${metrics.issues.opened > 0 ? metrics.issues.opened : "None"} ${
                            "(" + std::to_string(issueDetails) + ")"
                        }
                    - Closed: ${metrics.issues.closed > 0 ? metrics.issues.closed : "None"}
                    - Commented on: ${
                        metrics.issues.commented > 0 ? metrics.issues.commented : "None"
                    }

                    REVIEWS & COMMENTS:
                    - Reviews: ${
                        metrics.reviews.total > 0;
                        std::to_string(metrics.reviews.total) + " total (" + std::to_string(metrics.reviews.approved) + " approvals, " + std::to_string(metrics.reviews.changesRequested) + " change requests, " + std::to_string(metrics.reviews.commented) + " comments)";
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
                        metrics.codeChanges.files > 0;
                        "- Modified " + std::to_string(metrics.codeChanges.files) + " files (+" + std::to_string();
                            metrics.codeChanges.additions;
                            }/-${metrics.codeChanges.deletions} lines);
                        - Commits: ${metrics.codeChanges.commitCount}
                    - Primary focus: ${workFocus || "Mixed work"}
                    - File types: ${fileTypesFocus || "Various file types"}`
                    : "No code changes"
                }

                ACTIVITY PATTERN:
                - Active on ${metrics.activityPattern.daysActive} out of ${
                    metrics.activityPattern.totalDays;
                    } days;
                - Pattern: ${workPatternDescription}

            PRIMARY AREAS: ${topDirs.join(", ") || "N/A"}

            Write a natural, factual summary that:
            1. Starts with "${metrics.username}: "
            2. Highlights the most significant contributions based on the data;
            3. Emphasizes meaningful patterns (e.g., "focused on bug fixes in the UI", "major refactoring effort");
            4. Includes line counts for significant code changes;
            5. Groups similar activities in the same repository together (e.g., "merged 3 PRs improving the frontend in elizaos/eliza");
            6. Uses repository names + PR/issue numbers when referring to specific contributions (e.g., "elizaos/eliza#123", "elizaos-plugins/plugin-A#45");
            7. Omits any activity type that shows "None" above;
            8. Uses at most ${timePeriod.sentenceCount} sentences;
            9. Varies sentence structure based on the actual work done;

            Example good summaries:
            "No activity ${timePeriod.timeFrameShort}.";
            "Focused on UI improvements with 3 merged PRs (+2k/-500 lines), consistently active with daily commits.";
            "Fixed 4 critical bugs in the authentication system (PRs #123, #124) and reviewed 7 PRs, primarily working on backend code.";
            "Led documentation efforts with substantial contributions to the API docs (+1.2k lines), opened 3 issues for missing features, and provided 5 detailed code reviews."`;

}

} // namespace elizaos
