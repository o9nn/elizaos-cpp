#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

;
;
;
;

/**
 * Update a repository's last fetched timestamp
 */
async )
    .where(eq(repositories.repoId, repoId));
}

/**
 * Ensure users exist in the database
 */
async >,
  botUsers?: string[],
) {
  // Filter out unknown or empty usernames
  const validUsers = Array.from(userData.entries())
    .filter(([username]) => username && username !== "unknown")
    .map(([username, { avatarUrl }]) => ({
      username,
      avatarUrl: avatarUrl || "",
      isBot: botUsers?.includes(username) ? 1 : 0,
      lastUpdated: new UTCDate().toISOString(),
    }));

  if (validUsers.length === 0) return;

  await db
    .insert(users)
    .values(validUsers)
    .onConflictDoUpdate({
      target: users.username,
      set: {
        avatarUrl: sql`COALESCE(excluded.avatar_url, ${users.avatarUrl})`,
        isBot: sql`excluded.is_bot`,
      },
    });
}

/**
 * Ensure labels exist in the database
 */
async >,
): Promise<Map<string, string>> {
  if (labelData.length === 0) return new Map();

  const labelsToInsert = labelData.map((label) => ({
    id: label.id,
    name: label.name,
    color: label.color,
    description: label.description || "",
    lastUpdated: new UTCDate().toISOString(),
  }));

  await db
    .insert(labels)
    .values(labelsToInsert)
    .onConflictDoUpdate({
      target: labels.id,
      set: {
        name: sql`excluded.name`,
        color: sql`excluded.color`,
        description: sql`excluded.description`,
      },
    });

  return new Map(labelData.map((label) => [label.id, label.name]));
}

/**
 * Store pull request to label relationships
 */
async ));

  await db
    .insert(pullRequestLabels)
    .values(relationships)
    .onConflictDoNothing({
      target: [pullRequestLabels.prId, pullRequestLabels.labelId],
    });
}

/**
 * Store issue to label relationships
 */
async ));

  await db
    .insert(issueLabels)
    .values(relationships)
    .onConflictDoNothing({
      target: [issueLabels.issueId, issueLabels.labelId],
    });
}

} // namespace elizaos
