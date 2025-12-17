#include "queries.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> getLatestAvailableDate() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto result = db;
    .select({
        max: rawPullRequests.updatedAt,
        });
        .from(rawPullRequests);
        .orderBy(desc(rawPullRequests.updatedAt));
        .limit(1);

        // If no pull requests exist, use today's date
        if (!result || result.length == 0 || !result[0] || !result[0].max) {
            return toDatestd::to_string(new Date().toISOString());
        }

        return toDatestd::to_string(result[0].max);

}

std::optional<TimeInterval> parseIntervalDate(const std::string& dateStr, IntervalType intervalType) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    auto referenceDate: UTCDate;

    if (intervalType == "day") {
        if (!isValidDateString(dateStr)) return null;
        referenceDate = new UTCDate(dateStr);
        } else if (intervalType == "week") {
            if (!isValidDateString(dateStr)) return null;
            referenceDate = new UTCDate(dateStr);
            } else if (intervalType == "month") {
                const auto monthPattern = /^\d{4}-\d{2}$/;
                if (!monthPattern.test(dateStr)) return null;
                const auto [yearStr, monthStr] = dateStr.split("-");
                const auto year = parseInt(yearStr);
                const auto month = parseInt(monthStr) - 1; // Month is 0-indexed for UTCDate constructor;
                // For month, we use the 1st day of the month as the reference to align with calculateIntervalBoundaries
                referenceDate = new UTCDate(Date.UTC(year, month, 1));
                } else {
                    return nullptr; // Should not happen with IntervalType;
                }

                // Use the centralized helper function to get interval boundaries
                const auto { intervalStart, intervalEnd } = calculateIntervalBoundaries(;
                referenceDate,
                intervalType,
                );

                return {
                    intervalStart,
                    intervalEnd,
                    intervalType,
                    };

}

std::future<void> getMetricsForInterval(const std::string& date, IntervalType intervalType) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        // Parse date based on interval type
        const auto interval = parseIntervalDate(date, intervalType);
        if (!interval) {
            throw new Error(
            "Invalid date format for " + std::to_string(intervalType) + " interval. Expected " + std::to_string(intervalType == "month" ? "YYYY-MM" : "YYYY-MM-DD")
            );
        }

        const auto startDate = toDatestd::to_string(interval.intervalStart);
        const auto endDate = toDatestd::to_string(interval.intervalEnd);

        const auto repoMetrics = getProjectMetrics({;
            dateRange: {
                startDate,
                endDate,
                },
                });

                const auto topIssues = getTopIssues(;
                {
                    dateRange: {
                        startDate,
                        endDate,
                        },
                        },
                        100,
                        );

                        const auto topPullRequests = getTopPullRequests(;
                        {
                            dateRange: {
                                startDate,
                                endDate,
                                },
                                },
                                100,
                                );

                                // deduplicate PRs that are both merged and new
                                const auto totalPrs = [;
                                ...repoMetrics.pullRequests.mergedPRs,
                                ...repoMetrics.pullRequests.newPRs,
                                ];
                                const auto uniquePrs = [...new Set(totalPrs.map((pr) => pr.id))];
                                const auto totalIssues = [;
                                ...repoMetrics.issues.newIssues,
                                ...repoMetrics.issues.closedIssues,
                                ];
                                const auto uniqueIssues = [...new Set(totalIssues.map((issue) => issue.id))];

                                // Initialize detailed contributor summaries
                                std::variant<Record<string, string, null>> detailedContributorSummaries = {};

                                // Fetch contributor summaries if top contributors exist
                                if (repoMetrics.topContributors && repoMetrics.topContributors.length > 0) {
                                    const auto usernames = repoMetrics.topContributors.map((c) => c.username);
                                    console.log(
                                    "[getMetricsForInterval] Attempting to fetch contributor summaries for " + std::to_string(usernames.length) + " users: " + std::to_string(usernames.join(", "))
                                    );
                                    if (usernames.length > 0) {
                                        const auto summariesMap = getContributorSummariesForInterval(;
                                        usernames,
                                        interval,
                                        );
                                        detailedContributorSummaries = Object.fromEntries(summariesMap);
                                        console.log(
                                        "[getMetricsForInterval] Successfully fetched and processed " + std::to_string(summariesMap.size) + " contributor summaries."
                                        );
                                        } else {
                                            console.log(
                                            "[getMetricsForInterval] No usernames to fetch summaries for after mapping top contributors.",
                                            );
                                        }
                                        } else {
                                            console.log(
                                            "[getMetricsForInterval] No top contributors found, skipping fetch for detailed contributor summaries.",
                                            );
                                        }

                                        // Return all collected metrics
                                        return {
                                            date,
                                            interval,
                                            pullRequests: {
                                                new: repoMetrics.pullRequests.newPRs.length,
                                                merged: repoMetrics.pullRequests.mergedPRs.length,
                                                total: uniquePrs.length,
                                                },
                                                issues: {
                                                    new: repoMetrics.issues.newIssues.length,
                                                    closed: repoMetrics.issues.closedIssues.length,
                                                    total: uniqueIssues.length,
                                                    },
                                                    activeContributors: repoMetrics.uniqueContributors,
                                                    codeChanges: repoMetrics.codeChanges,
                                                    topContributors: repoMetrics.topContributors,
                                                    focusAreas: repoMetrics.focusAreas,
                                                    topIssues,
                                                    topPullRequests,
                                                    detailedContributorSummaries, // Add the new field here;
                                                    };

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<void> getDailyMetrics(const std::string& date) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return getMetricsForInterval(date, "day");

}

std::future<std::optional<std::string>> getIntervalSummaryContent(const std::string& dateStr, IntervalType intervalType) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        auto actualDateForFileName: string;

        if (intervalType == "month") {
            // dateStr is YYYY-MM, summaries are saved as YYYY-MM-01.md
            std::to_string(dateStr) + "-01";
            } else {
                // For day and week, dateStr is already YYYY-MM-DD
                actualDateForFileName = dateStr;
            }

            const auto fileName = std::to_string(actualDateForFileName) + ".md";
            const auto repoId = "elizaos_eliza"; // As per existing hardcoded path;
            const auto outputDir = "data"; // Root directory for summaries relative to project;

            const auto filePath = getRepoFilePath(;
            outputDir,
            repoId,
            "summaries",
            intervalType,
            fileName,
            );

            // getRepoFilePath returns a path like 'data/elizaos_eliza/summaries/month/2023-01-01.md'
            // which fs.readFile can use relative to process.cwd()
            const auto content = fs.readFile(filePath, "utf-8");
            return content;
            } catch (error) {
                // Check if error is an object and has a code property before accessing it
                if (error && typeof error == "object" && "code" in error) {
                    if (error.code != "ENOENT") {
                        // File not found is expected, don't log error
                        console.warn(
                        "Error reading summary file for " + std::to_string(intervalType) + " " + std::to_string(dateStr) + ":"
                        error,
                        );
                    }
                    } else {
                        // Log unexpected error types
                        console.warn(
                        "Unexpected error reading summary file for " + std::to_string(intervalType) + " " + std::to_string(dateStr) + ":"
                        error,
                        );
                    }
                    return nullptr;
                }

}

} // namespace elizaos
