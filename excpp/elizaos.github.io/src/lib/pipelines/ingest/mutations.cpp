#include "mutations.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> updateRepositoryLastFetched(const std::string& repoId, const std::string& timestamp) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    db;
    .update(repositories);
    .set({
        lastFetchedAt: timestamp,
        lastUpdated: new UTCDate().toISOString(),
        });
        .where(eq(repositories.repoId, repoId));

}

std::future<void> ensureUsersExist(std::optional<std::unordered_map<std::string, std::any>> userData, std::optional<std::vector<std::string>> botUsers) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Filter out unknown or empty usernames
    const auto validUsers = Array.from(userData.entries());
    .filter(([username]) => username && username != "unknown");
    .map(([username, { avatarUrl }]) => ({
        username,
        avatarUrl: avatarUrl || "",
        isBot: (std::find(botUsers.begin(), botUsers.end(), username) != botUsers.end()) ? 1 : 0,
        lastUpdated: new UTCDate().toISOString(),
        }));

        if (validUsers.length == 0) return;

        db;
        .insert(users);
        .values(validUsers);
        .onConflictDoUpdate({
            target: users.username,
            set: {
                "avatarUrl: sql" + "COALESCE(excluded.avatar_url, " + users.avatarUrl + ")"
                "isBot: sql" + "excluded.is_bot"
                },
                });

}

std::future<std::unordered_map<std::string, std::string>> ensureLabelsExist(std::optional<std::variant<Array<{
    id: string;
    name: string;
    color: string;
    description: string, null;
  }>>> labelData) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (labelData.length == 0) return new Map();

    const auto labelsToInsert = labelData.map((label) => ({;
        id: label.id,
        name: label.name,
        color: label.color,
        description: label.description || "",
        lastUpdated: new UTCDate().toISOString(),
        }));

        db;
        .insert(labels);
        .values(labelsToInsert);
        .onConflictDoUpdate({
            target: labels.id,
            set: {
                "name: sql" + "excluded.name"
                "color: sql" + "excluded.color"
                "description: sql" + "excluded.description"
                },
                });

                return new Map(labelData.map((label) => [label.id, label.name]));

}

std::future<void> storePRLabels(const std::string& prId, const std::vector<std::string>& labelIds) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (labelIds.length == 0) return;

    const auto relationships = labelIds.map((labelId) => ({;
        prId,
        labelId,
        lastUpdated: new UTCDate().toISOString(),
        }));

        db;
        .insert(pullRequestLabels);
        .values(relationships);
        .onConflictDoNothing({
            target: [pullRequestLabels.prId, pullRequestLabels.labelId],
            });

}

std::future<void> storeIssueLabels(const std::string& issueId, const std::vector<std::string>& labelIds) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (labelIds.length == 0) return;

    const auto relationships = labelIds.map((labelId) => ({;
        issueId,
        labelId,
        lastUpdated: new UTCDate().toISOString(),
        }));

        db;
        .insert(issueLabels);
        .values(relationships);
        .onConflictDoNothing({
            target: [issueLabels.issueId, issueLabels.labelId],
            });

}

} // namespace elizaos
