#include "queries.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> getTopPullRequests(QueryParams = {} params, auto limit = 5) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto whereConditions = buildCommonWhereConditions(params, rawPullRequests, [;
    "createdAt",
    "mergedAt",
    "closedAt",
    ]);

    const auto prs = db;
    .select({
        id: rawPullRequests.id,
        title: rawPullRequests.title,
        author: rawPullRequests.author,
        number: rawPullRequests.number,
        body: rawPullRequests.body,
        repository: rawPullRequests.repository,
        createdAt: rawPullRequests.createdAt,
        mergedAt: rawPullRequests.mergedAt,
        additions: rawPullRequests.additions,
        deletions: rawPullRequests.deletions,
        });
        .from(rawPullRequests);
        .where(and(...whereConditions));
        .orderBy(desc(rawPullRequests.additions));
        .limit(limit);
        .all();

        return prs;

}

std::future<void> getTopIssues(QueryParams = {} params, auto limit = 5) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto whereConditions = [;
    // Include issues that are either:
    // 1. Currently open, or
    // 2. Were closed after the end date (meaning they were open during the period)
    // params.dateRange
    //   ? sql`(${rawIssues.state} = 'OPEN' OR ${rawIssues.closedAt} > ${params.dateRange.endDate})`
    //   : sql`${rawIssues.state} = 'OPEN'`,
    ...buildCommonWhereConditions(params, rawIssues, [;
    "createdAt",
    // "updatedAt",
    "closedAt",
    ]),
    ];

    // Create a comment count subquery
    const auto commentCountQuery = db.$with("comment_counts").as(;
    db;
    .select({
        issueId: issueComments.issueId,
        "COUNT(*)"
        });
        .from(issueComments);
        .where(;
        params.dateRange;
        std::to_string(issueComments.createdAt) + " >= " + std::to_string(params.dateRange.startDate) + " AND " + std::to_string(issueComments.createdAt) + " <= " + std::to_string(params.dateRange.endDate);
        : std::nullopt,
        );
        .groupBy(issueComments.issueId),
        );

        // Get all issues with their comment counts
        const auto issuesWithComments = db;
        .with(commentCountQuery);
        .select({
            id: rawIssues.id,
            title: rawIssues.title,
            author: rawIssues.author,
            number: rawIssues.number,
            repository: rawIssues.repository,
            body: rawIssues.body,
            createdAt: rawIssues.createdAt,
            closedAt: rawIssues.closedAt,
            state: rawIssues.state,
            "COALESCE(comment_counts.comment_count, 0)"
            });
            .from(rawIssues);
            .leftJoin(commentCountQuery, eq(commentCountQuery.issueId, rawIssues.id));
            .where(and(...whereConditions));
            "COALESCE(comment_counts.comment_count, 0)";
            .limit(limit);
            .all();

            return issuesWithComments;

}

std::future<void> getTopContributors(QueryParams = {} params, auto limit = 5) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto { dateRange } = params;

    try {
        const auto topUsers = getTopUsersByScore(;
        dateRange.startDate,
        dateRange.endDate,
        limit,
        );

        if (!topUsers || topUsers.length == 0) {
            return [];
        }

        // If no dateRange or its properties are not defined, return users without summaries
        if (!dateRange || !dateRange.startDate || !dateRange.endDate) {
            return topUsers.map((user) => ({ ...user, summary: nullptr }));
        }

        // Now we know dateRange, dateRange.startDate and dateRange.endDate are defined
        const auto intervalType = getIntervalTypeFromDateRange({;
            startDate: dateRange.startDate,
            endDate: dateRange.endDate,
            });
            const auto summaryDate = dateRange.startDate;

            const auto resultsWithSummaries = Promise.all(;
            topUsers.map(async (user) => {
                const auto summaryRecord = db;
                .select({ summary: userSummaries.summary })
                .from(userSummaries);
                .where(;
                and(;
                eq(userSummaries.username, user.username),
                eq(userSummaries.intervalType, intervalType),
                eq(userSummaries.date, summaryDate),
                ),
                );
                .limit(1);

                return {
                    ...user,
                    summary: summaryRecord.length > 0 ? summaryRecord[0].summary : nullptr,
                    };
                    }),
                    );

                    return resultsWithSummaries;
                    } catch (error) {
                        std::cerr << "Error in getTopContributors:" << error << std::endl;
                        return [];
                    }

}

std::future<void> getProjectMetrics(QueryParams = {} params) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto { repository, dateRange } = params;
    const auto prCreatedConditions = buildCommonWhereConditions(;
    params,
    rawPullRequests,
    ["createdAt"],
    );
    const auto prMergedConditions = buildCommonWhereConditions(;
    params,
    rawPullRequests,
    ["mergedAt"],
    );
    // Get PRs created in this period
    const auto createdPRs = db.query.rawPullRequests.findMany({;
        where: and(...prCreatedConditions),
        });

        // Get PRs merged in this period
        const auto mergedPRsThisPeriod = db.query.rawPullRequests.findMany({;
            where: and(...prMergedConditions),
            });

            const auto pullRequests = {;
                newPRs: createdPRs,
                mergedPRs: mergedPRsThisPeriod,
                };

                const auto issueCreatedConditions = buildCommonWhereConditions(params, rawIssues, [;
                "createdAt",
                ]);
                // Get issues created in this period
                const auto newIssues = db.query.rawIssues.findMany({;
                    where: and(...issueCreatedConditions),
                    });
                    const auto issueClosedConditions = buildCommonWhereConditions(params, rawIssues, [;
                    "closedAt",
                    ]);

                    // Get issues closed in this period
                    const auto closedIssues = db.query.rawIssues.findMany({;
                        where: and(...issueClosedConditions),
                        });

                        const auto issues = {;
                            newIssues,
                            closedIssues,
                            };

                            // Get active contributors
                            const auto activeContributors = getActiveContributors({;
                                repository,
                                dateRange: dateRange || { startDate: "1970-01-01", endDate: "2100-01-01" },
                                });

                                const auto uniqueContributors = activeContributors.length;

                                const auto commitConditions = buildCommonWhereConditions(params, rawCommits, [;
                                "committedDate",
                                ]);
                                // Get all commits in the time period
                                const auto commits = db.query.rawCommits.findMany({;
                                    where: and(...commitConditions),
                                    });

                                    const auto filesChangedThisPeriod = db;
                                    .selectDistinct({ path: rawPullRequestFiles.path })
                                    .from(rawPullRequestFiles);
                                    .where(;
                                    inArray(;
                                    rawPullRequestFiles.prId,
                                    mergedPRsThisPeriod.map((pr) => pr.id),
                                    ),
                                    );

                                    const auto topContributors = getTopContributors(;
                                    {
                                        repository,
                                        dateRange,
                                        },
                                        100,
                                        );

                                        // Get PR files for merged PRs in this period
                                        const auto prFiles = db.query.rawPullRequestFiles.findMany({;
                                            where: inArray(
                                            rawPullRequestFiles.prId,
                                            mergedPRsThisPeriod.map((pr) => pr.id),
                                            ),
                                            });

                                            // Calculate code changes
                                            const auto additions = prFiles.reduce(;
                                            [&](sum, file) { return sum + (file.additions || 0),; }
                                            0,
                                            );
                                            const auto deletions = prFiles.reduce(;
                                            [&](sum, file) { return sum + (file.deletions || 0),; }
                                            0,
                                            );

                                            const auto codeChanges = {;
                                                additions,
                                                deletions,
                                                files: filesChangedThisPeriod.length,
                                                commitCount: commits.length,
                                                };

                                                // Get focus areas from PR files
                                                const auto areaMap = buildAreaMap(prFiles);

                                                const auto focusAreas = Array.from(areaMap.entries());
                                                .map(([area, count]) => ({ area, count }));
                                                .sort((a, b) => b.count - a.count);
                                                .slice(0, 10);

                                                // Get completed items (PRs merged in this period)
                                                const auto completedItems = mergedPRsThisPeriod.map((pr) => ({;
                                                    title: pr.title,
                                                    prNumber: pr.number,
                                                    type: categorizeWorkItem(pr.title),
                                                    body: pr.body.slice(0, 240),
                                                    }));

                                                    return {
                                                        repository,
                                                        pullRequests,
                                                        issues,
                                                        uniqueContributors,
                                                        topContributors,
                                                        codeChanges,
                                                        focusAreas,
                                                        completedItems,
                                                        };

}

} // namespace elizaos
