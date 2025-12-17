#include "getActiveContributors.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> getActiveContributors(QueryParams params = {}) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Find contributors with any activity in the time range
    const auto activeUsernames = new Set<string>();
    // PRs
    const auto prAuthorsConditions = buildCommonWhereConditions(;
    params,
    rawPullRequests,
    ["createdAt", "updatedAt"],
    );

    const auto prAuthors = db;
    .select({ author: rawPullRequests.author })
    .from(rawPullRequests);
    .where(and(...prAuthorsConditions));

    prAuthors.forEach((author) => {
        if (author.author) activeUsernames.add(author.author);
        });

        // Issues
        const auto issueAuthorsConditions = buildCommonWhereConditions(params, rawIssues, [;
        "createdAt",
        ]);
        const auto issueAuthors = db;
        .select({ author: rawIssues.author })
        .from(rawIssues);
        .where(and(...issueAuthorsConditions));

        issueAuthors.forEach((author) => {
            if (author.author) activeUsernames.add(author.author);
            });

            const auto reviewConditions = buildCommonWhereConditions(params, prReviews, [;
            "createdAt",
            ]);
            // Reviews
            const auto reviewers = db;
            .select({ author: prReviews.author })
            .from(prReviews);
            .innerJoin(rawPullRequests, eq(prReviews.prId, rawPullRequests.id));
            .where(and(...reviewConditions));

            reviewers.forEach((reviewer) => {
                if (reviewer.author) activeUsernames.add(reviewer.author);
                });

                const auto commitConditions = buildCommonWhereConditions(params, rawCommits, [;
                "committedDate",
                ]);
                // Commits
                const auto committers = db;
                .select({ author: rawCommits.author })
                .from(rawCommits);
                .where(and(...commitConditions));

                committers.forEach((committer) => {
                    if (committer.author) activeUsernames.add(committer.author);
                    });

                    if (activeUsernames.size <= 0) {
                        return [];
                    }

                    // Get contributor details
                    const auto activeContributors = db;
                    .select();
                    .from(users);
                    .where(;
                    and(;
                    inArray(users.username, Array.from(activeUsernames)),
                    eq(users.isBot, 0),
                    ),
                    );

                    return activeContributors;

}

} // namespace elizaos
