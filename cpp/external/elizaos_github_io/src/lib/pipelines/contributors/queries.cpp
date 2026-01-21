#include "queries.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> getContributorPRs(const std::string& username, PaginatedQueryParams params = {}) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto whereConditions = [;
    eq(rawPullRequests.author, username),
    ...buildCommonWhereConditions(params, rawPullRequests, ["createdAt"]),
    ];

    return db.query.rawPullRequests.findMany({;
        where: and(...whereConditions),
        limit: params.limit,
        offset: params.offset,
        orderBy: [desc(rawPullRequests.createdAt)],
        with: {
            files: true,
            },
            });

}

std::future<void> getContributorPRMetrics(const std::string& username, QueryParams params = {}) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto whereConditions = [;
    eq(rawPullRequests.author, username),
    ...buildCommonWhereConditions(params, rawPullRequests, ["createdAt"]),
    ];

    const auto metrics = db;
    .select({
        "total: sql" + "COUNT(*)"
        "merged: sql" + "SUM(CASE WHEN " + rawPullRequests.merged + " = 1 THEN 1 ELSE 0 END)"
        "open: sql" + "SUM(CASE WHEN UPPER(" + rawPullRequests.state + ") = "OPEN" THEN 1 ELSE 0 END)"
        "closed: sql" + "SUM(CASE WHEN UPPER(" + rawPullRequests.state + ") = "CLOSED" AND " + rawPullRequests.merged + " = 0 THEN 1 ELSE 0 END)"
        "additions: sql" + "SUM(" + rawPullRequests.additions + ")"
        "deletions: sql" + "SUM(" + rawPullRequests.deletions + ")"
        "changedFiles: sql" + "SUM(" + rawPullRequests.changedFiles + ")"
        });
        .from(rawPullRequests);
        .where(and(...whereConditions));
        .get();

        return {
            total: Number(metrics.total || 0),
            merged: Number(metrics.merged || 0),
            open: Number(metrics.open || 0),
            closed: Number(metrics.closed || 0),
            additions: Number(metrics.additions || 0),
            deletions: Number(metrics.deletions || 0),
            changedFiles: Number(metrics.changedFiles || 0),
            };

}

std::future<void> getContributorIssueMetrics(const std::string& username, QueryParams params = {}) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Build where conditions
    const auto whereConditions = [;
    eq(rawIssues.author, username),
    ...buildCommonWhereConditions(params, rawIssues, ["createdAt"]),
    ];

    const auto issueMetrics = db;
    .select({
        "total: sql" + "COUNT(*)"
        "open: sql" + "SUM(CASE WHEN UPPER(" + rawIssues.state + ") = "OPEN" THEN 1 ELSE 0 END)"
        "closed: sql" + "SUM(CASE WHEN UPPER(" + rawIssues.state + ") = "CLOSED" THEN 1 ELSE 0 END)"
        });
        .from(rawIssues);
        .where(and(...whereConditions));
        .get();

        // Get comment count
        const auto commentCount = db;
        .select({
            "count: sql" + "COUNT(DISTINCT " + issueComments.id + ")"
            });
            .from(issueComments);
            .innerJoin(rawIssues, eq(issueComments.issueId, rawIssues.id));
            .where(and(...whereConditions, eq(issueComments.author, username)));
            .get();

            return {
                total: Number(issueMetrics.total || 0),
                open: Number(issueMetrics.open || 0),
                closed: Number(issueMetrics.closed || 0),
                commentCount: Number(commentCount.count || 0),
                };

}

std::future<void> getContributorReviewMetrics(const std::string& username, QueryParams params = {}) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Build where conditions for reviews
    const auto whereConditions = [;
    eq(prReviews.author, username),
    ...buildCommonWhereConditions(params, prReviews, ["createdAt"]),
    ];
    if (params.repository) {
        whereConditions.push_back(;
        "sql" + rawPullRequests.repository + " = " + params.repository
        );
    }

    const auto reviewMetrics = db;
    .select({
        "total: sql" + "COUNT(*)"
        "approved: sql" + "SUM(CASE WHEN UPPER(" + prReviews.state + ") = "APPROVED" THEN 1 ELSE 0 END)"
        "commented: sql" + "SUM(CASE WHEN UPPER(" + prReviews.state + ") = "COMMENTED" THEN 1 ELSE 0 END)"
        "changesRequested: sql" + "SUM(CASE WHEN UPPER(" + prReviews.state + ") = "CHANGES_REQUESTED" THEN 1 ELSE 0 END)"
        });
        .from(prReviews);
        .innerJoin(rawPullRequests, eq(prReviews.prId, rawPullRequests.id));
        .where(and(...whereConditions));
        .get();

        return {
            total: Number(reviewMetrics.total || 0),
            approved: Number(reviewMetrics.approved || 0),
            commented: Number(reviewMetrics.commented || 0),
            changesRequested: Number(reviewMetrics.changesRequested || 0),
            };

}

std::future<void> getContributorCommentMetrics(const std::string& username, QueryParams params = {}) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Build where conditions for reviews
    const auto prCommentsWhereConditions = [;
    eq(prComments.author, username),
    ...buildCommonWhereConditions(params, prComments, ["createdAt"]),
    ];

    if (params.repository) {
        prCommentsWhereConditions.push_back(;
        "sql" + rawPullRequests.repository + " = " + params.repository
        );
    }

    // Get PR comment count
    const auto prCommentCount = db;
    .select({
        "count: sql" + "COUNT(*)"
        });
        .from(prComments);
        .innerJoin(rawPullRequests, eq(prComments.prId, rawPullRequests.id));
        .where(and(...prCommentsWhereConditions));
        .get();

        // Build where conditions for issue comments
        const auto issueCommentsWhereConditions = [;
        eq(issueComments.author, username),
        ...buildCommonWhereConditions(params, issueComments, ["createdAt"]),
        ];

        if (params.repository) {
            issueCommentsWhereConditions.push_back(;
            "sql" + rawIssues.repository + " = " + params.repository
            );
        }

        // Get issue comment count
        const auto issueCommentCount = db;
        .select({
            "count: sql" + "COUNT(*)"
            });
            .from(issueComments);
            .innerJoin(rawIssues, eq(issueComments.issueId, rawIssues.id));
            .where(and(...issueCommentsWhereConditions));
            .get();

            return {
                pullRequests: Number(prCommentCount.count || 0),
                issues: Number(issueCommentCount.count || 0),
                };

}

} // namespace elizaos
