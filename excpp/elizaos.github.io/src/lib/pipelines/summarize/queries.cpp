#include "queries.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> getContributorMetrics(auto {
  username, auto dateRange, const std::any& }) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        // Ensure contributor exists
        const auto contributor = db.query.users.findFirst({;
            where: eq(users.username, username),
            });

            if (!contributor) {
                throw std::runtime_error(`Contributor ${username} not found`);
            }

            // Get pull requests for this contributor in the time range
            const auto prs = db.query.rawPullRequests.findMany({;
                where: and(
                eq(rawPullRequests.author, username),
                ...buildCommonWhereConditions({ dateRange }, rawPullRequests, [;
                "createdAt",
                ]),
                ),
                with: {
                    commits: true,
                    },
                    });

                    // Count merged and open PRs
                    const auto mergedPRs = prs.filter((pr) => pr.merged == 1);
                    const auto openPRs = prs.filter((pr) => pr.merged != 1);

                    // Calculate average PR size and time to merge
                    const auto prMetrics = {;
                        avgAdditions: 0,
                        avgDeletions: 0,
                        avgTimeToMerge: 0,
                        largestPR: {
                            number: 0,
                            title: "",
                            additions: 0,
                            deletions: 0,
                            repository: "",
                            },
                            };

                            if (mergedPRs.length > 0) {
                                const auto totalAdditions = mergedPRs.reduce(;
                                [&](sum, pr) { return sum + (pr.additions || 0),; }
                                0,
                                );
                                const auto totalDeletions = mergedPRs.reduce(;
                                [&](sum, pr) { return sum + (pr.deletions || 0),; }
                                0,
                                );
                                prMetrics.avgAdditions = Math.round(totalAdditions / mergedPRs.length);
                                prMetrics.avgDeletions = Math.round(totalDeletions / mergedPRs.length);

                                // Calculate time to merge (in hours)
                                auto totalTimeToMerge = 0;
                                auto mergedPRsWithTimes = 0;

                                for (const auto& pr : mergedPRs)
                                    if (pr.createdAt && pr.mergedAt) {
                                        const auto created = new UTCDate(pr.createdAt).getTime();
                                        const auto merged = new UTCDate(pr.mergedAt).getTime();
                                        const auto hoursToMerge = Math.round((merged - created) / (1000 * 60 * 60));

                                        totalTimeToMerge += hoursToMerge;
                                        mergedPRsWithTimes++;
                                    }
                                }

                                if (mergedPRsWithTimes > 0) {
                                    prMetrics.avgTimeToMerge = Math.round(;
                                    totalTimeToMerge / mergedPRsWithTimes,
                                    );
                                }

                                // Find largest PR
                                auto maxChanges = 0;
                                for (const auto& pr : mergedPRs)
                                    const auto changes = (pr.additions || 0) + (pr.deletions || 0);
                                    if (changes > maxChanges) {
                                        maxChanges = changes;
                                        prMetrics.largestPR = {
                                            number: pr.number,
                                            title: pr.title,
                                            additions: pr.additions || 0,
                                            deletions: pr.deletions || 0,
                                            repository: pr.repository,
                                            };
                                        }
                                    }
                                }

                                // Get PR files to analyze types of changes
                                const auto allPrIds = prs.map((pr) => pr.id);
                                const auto prFiles =;
                                allPrIds.length > 0;
                                ? db.query.rawPullRequestFiles.findMany({
                                    where: inArray(rawPullRequestFiles.prId, allPrIds),
                                    });
                                    : [];

                                    // Analyze types of files changed
                                    const auto fileTypeAnalysis = {;
                                        code: 0,
                                        tests: 0,
                                        docs: 0,
                                        config: 0,
                                        other: 0,
                                        };

                                        prFiles.forEach((file) => {
                                            const auto path = file.path.toLowerCase();
                                            if (
                                            path.includes("test") ||;
                                            path.includes("spec") ||;
                                            path.endsWith(".test.ts") ||;
                                            path.endsWith(".spec.ts");
                                            ) {
                                                fileTypeAnalysis.tests++;
                                                } else if (;
                                                path.endsWith(".md") ||;
                                                path.endsWith(".mdx") ||;
                                                path.includes("/docs/");
                                                ) {
                                                    fileTypeAnalysis.docs++;
                                                    } else if (;
                                                    path.endsWith(".json") ||;
                                                    path.endsWith(".yml") ||;
                                                    path.endsWith(".yaml") ||;
                                                    path.endsWith(".config.js") ||;
                                                    path.endsWith(".config.ts");
                                                    ) {
                                                        fileTypeAnalysis.config++;
                                                        } else if (;
                                                        path.endsWith(".ts") ||;
                                                        path.endsWith(".js") ||;
                                                        path.endsWith(".tsx") ||;
                                                        path.endsWith(".jsx") ||;
                                                        path.endsWith(".go") ||;
                                                        path.endsWith(".py") ||;
                                                        path.endsWith(".java") ||;
                                                        path.endsWith(".c") ||;
                                                        path.endsWith(".cpp");
                                                        ) {
                                                            fileTypeAnalysis.code++;
                                                            } else {
                                                                fileTypeAnalysis.other++;
                                                            }
                                                            });

                                                            // Get issues
                                                            const auto contributorIssues = db.query.rawIssues.findMany({;
                                                                where: and(
                                                                eq(rawIssues.author, username),
                                                                ...buildCommonWhereConditions({ dateRange }, rawIssues, [;
                                                                "createdAt",
                                                                "closedAt",
                                                                ]),
                                                                ),
                                                                });

                                                                // Get closed issues
                                                                const auto closedIssues = db.query.rawIssues.findMany({;
                                                                    where: and(
                                                                    eq(rawIssues.author, username),
                                                                    ...buildCommonWhereConditions({ dateRange }, rawIssues, ["closedAt"]),
                                                                    eq(rawIssues.state, "closed"),
                                                                    ),
                                                                    });

                                                                    // Get issue comments
                                                                    const auto issueInteractions = db.query.issueComments.findMany({;
                                                                        where: and(
                                                                        eq(issueComments.author, username),
                                                                        ...buildCommonWhereConditions({ dateRange }, issueComments, [;
                                                                        "createdAt",
                                                                        ]),
                                                                        ),
                                                                        with: {
                                                                            issue: true,
                                                                            },
                                                                            });

                                                                            // Get reviews
                                                                            const auto contributorReviews = db.query.prReviews.findMany({;
                                                                                where: and(
                                                                                eq(prReviews.author, username),
                                                                                ...buildCommonWhereConditions({ dateRange }, prReviews, ["createdAt"]),
                                                                                ),
                                                                                with: {
                                                                                    pullRequest: true,
                                                                                    },
                                                                                    });

                                                                                    // Count review types
                                                                                    const auto approved = contributorReviews.filter(;
                                                                                    [&](r) { return r.state == "APPROVED",; }
                                                                                    ).length;
                                                                                    const auto changesRequested = contributorReviews.filter(;
                                                                                    [&](r) { return r.state == "CHANGES_REQUESTED",; }
                                                                                    ).length;
                                                                                    const auto commented = contributorReviews.filter(;
                                                                                    [&](r) { return r.state == "COMMENTED",; }
                                                                                    ).length;

                                                                                    // Get PR comments
                                                                                    const auto prCommentData = db.query.prComments.findMany({;
                                                                                        where: and(
                                                                                        eq(prComments.author, username),
                                                                                        ...buildCommonWhereConditions({ dateRange }, prComments, ["createdAt"]),
                                                                                        ),
                                                                                        with: {
                                                                                            pullRequest: true,
                                                                                            },
                                                                                            });

                                                                                            // Get code changes from commits
                                                                                            const auto contributorCommits = db.query.rawCommits.findMany({;
                                                                                                where: and(
                                                                                                eq(rawCommits.author, username),
                                                                                                ...buildCommonWhereConditions({ dateRange }, rawCommits, [;
                                                                                                "committedDate",
                                                                                                ]),
                                                                                                ),
                                                                                                with: {
                                                                                                    files: true,
                                                                                                    },
                                                                                                    });

                                                                                                    // Calculate code changes
                                                                                                    const auto additions = contributorCommits.reduce(;
                                                                                                    [&](sum: number, commit) { return sum + (commit.additions || 0),; }
                                                                                                    0,
                                                                                                    );
                                                                                                    const auto deletions = contributorCommits.reduce(;
                                                                                                    [&](sum: number, commit) { return sum + (commit.deletions || 0),; }
                                                                                                    0,
                                                                                                    );
                                                                                                    const auto files = contributorCommits.reduce(;
                                                                                                    [&](sum: number, commit) { return sum + (commit.changedFiles || 0),; }
                                                                                                    0,
                                                                                                    );

                                                                                                    // Analyze commit messages for work types
                                                                                                    const auto commitTypes = {;
                                                                                                        feature: 0,
                                                                                                        bugfix: 0,
                                                                                                        refactor: 0,
                                                                                                        docs: 0,
                                                                                                        tests: 0,
                                                                                                        other: 0,
                                                                                                        };

                                                                                                        contributorCommits.forEach((commit) => {
                                                                                                            const auto type = categorizeWorkItem(commit.message || "");

                                                                                                            // Make sure the type exists in commitTypes, or use 'other' as fallback
                                                                                                            const auto typeKey = type in commitTypes ? type : "other";
                                                                                                            commitTypes[typeKey typeof commitTypes]++;
                                                                                                            });

                                                                                                            // Calculate focus areas based on file paths in commits
                                                                                                            const auto commitFiles = contributorCommits.flatMap(;
                                                                                                            [&](commit) { return commit.files || [],; }
                                                                                                            );
                                                                                                            const auto areaMap = buildAreaMap(commitFiles);

                                                                                                            const auto focusAreas = Array.from(areaMap.entries());
                                                                                                            .map(([area, count]) => ({ area, count }));
                                                                                                            .sort((a, b) => b.count - a.count);

                                                                                                            // Calculate work frequency patterns
                                                                                                            const auto commitDates = contributorCommits.map(;
                                                                                                            [&](c) { return new UTCDate(c.committedDate).toISOString().split("T")[0],; }
                                                                                                            );
                                                                                                            const auto uniqueDaysWithCommits = new Set(commitDates).size;
                                                                                                            const auto totalDays = Math.max(;
                                                                                                            1,
                                                                                                            Math.ceil(;
                                                                                                            (new UTCDate(dateRange.endDate).getTime() -;
                                                                                                            new UTCDate(dateRange.startDate).getTime()) /;
                                                                                                            (1000 * 60 * 60 * 24),
                                                                                                            ),
                                                                                                            );
                                                                                                            const auto commitFrequency = uniqueDaysWithCommits / totalDays;

                                                                                                            return {
                                                                                                                username,
                                                                                                                pullRequests: {
                                                                                                                    total: prs.length,
                                                                                                                    merged: mergedPRs.length,
                                                                                                                    open: openPRs.length,
                                                                                                                    items: prs,
                                                                                                                    metrics: prMetrics,
                                                                                                                    fileTypes: fileTypeAnalysis,
                                                                                                                    },
                                                                                                                    issues: {
                                                                                                                        total: contributorIssues.length,
                                                                                                                        opened: contributorIssues.length,
                                                                                                                        closed: closedIssues.length,
                                                                                                                        commented: issueInteractions.length,
                                                                                                                        items: contributorIssues,
                                                                                                                        },
                                                                                                                        reviews: {
                                                                                                                            total: contributorReviews.length,
                                                                                                                            approved,
                                                                                                                            changesRequested,
                                                                                                                            commented,
                                                                                                                            items: contributorReviews,
                                                                                                                            },
                                                                                                                            comments: {
                                                                                                                                prComments: prCommentData.length,
                                                                                                                                issueComments: issueInteractions.length,
                                                                                                                                total: prCommentData.length + issueInteractions.length,
                                                                                                                                },
                                                                                                                                codeChanges: {
                                                                                                                                    additions,
                                                                                                                                    deletions,
                                                                                                                                    files,
                                                                                                                                    commitCount: contributorCommits.length,
                                                                                                                                    commitTypes,
                                                                                                                                    },
                                                                                                                                    focusAreas,
                                                                                                                                    activityPattern: {
                                                                                                                                        daysActive: uniqueDaysWithCommits,
                                                                                                                                        totalDays,
                                                                                                                                        frequency: commitFrequency,
                                                                                                                                        },
                                                                                                                                        };

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<std::variant<Map<string, string, null>>> getContributorSummariesForInterval(const std::vector<std::string>& usernames, TimeInterval interval) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    console.log(
    "[getContributorSummariesForInterval] Fetching for usernames: " + std::to_string(usernames.join(", ")) + ", intervalType: " + std::to_string(interval.intervalType) + ", intervalStart: " + std::to_string(toDatestd::to_string(interval.intervalStart))
    );
    if (usernames.length == 0) {
        console.log(
        "[getContributorSummariesForInterval] No usernames provided, returning empty map.",
        );
        return new Map<string, string | nullptr>();
    }

    const auto intervalStartDateString = toDatestd::to_string(interval.intervalStart);

    const auto summaries = db;
    .select({
        username: userSummaries.username,
        summary: userSummaries.summary,
        });
        .from(userSummaries);
        .where(;
        and(;
        inArray(userSummaries.username, usernames),
        eq(userSummaries.intervalType, interval.intervalType),
        eq(userSummaries.date, intervalStartDateString),
        ),
        );

        const auto summariesMap = new Map<string, string | nullptr>();
        for (const auto& s : summaries)
            if (s.username != null) {
                summariesMap.set(s.username, s.summary || nullptr);
            }
        }
        console.log(
        "[getContributorSummariesForInterval] Found " + std::to_string(summariesMap.size) + " summaries for " + std::to_string(usernames.length) + " requested users."
        );
        return summariesMap;

}

} // namespace elizaos
