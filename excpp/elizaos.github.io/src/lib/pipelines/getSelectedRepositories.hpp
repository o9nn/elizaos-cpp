#include ".typeHelpers.hpp"
#include "types.hpp"
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
;
;

/**
 * Common pipeline step to fetch and filter repositories based on context
 */
const getSelectedRepositories = createStep(
  "getSelectedRepositories",
  async (_, { repoId, logger, config }: RepoPipelineContext) => {
    // Filter repositories
    const configRepos = config.repositories;
    logger?.info(`Found ${configRepos.length} configured repositories`, {
      configRepos,
    });
    for (const repo of configRepos) {
      await registerRepository(repo.owner, repo.name);
    }
    // Fetch all repositories
    const repos = await db
      .select({ repoId: repositories.repoId })
      .from(repositories)
      .where(repoId ? eq(repositories.repoId, repoId) : undefined);
    logger?.info(`Found ${repos.length} repositories in DB`, {
      repos: repos.map((r) => r.repoId),
    });

    const selectedRepos = repos
      .map((repo) => {
        const configRepo = configRepos.find(
          (r) => `${r.owner}/${r.name}` === repo.repoId,
        );
        if (!configRepo) {
          logger?.warn(`Repository ${repo.repoId} not found in config`, {
            repo,
          });
          return null;
        }
        return {
          repoId: `${configRepo.owner}/${configRepo.name}`,
          owner: configRepo.owner,
          name: configRepo.name,
          defaultBranch: configRepo.defaultBranch,
        };
      })
      .filter(isNotNullOrUndefined);
    logger?.info(`Filtering for configured repositories`, { selectedRepos });

    return selectedRepos;
  },
); /**
 * Register or update a repository in the database
 */

async /${name}`,
      lastUpdated: new UTCDate().toISOString(),
    })
    .onConflictDoUpdate({
      target: repositories.repoId,
      set: {
        owner,
        name,
        lastUpdated: new UTCDate().toISOString(),
      },
    });

  return { repoId: `${owner}/${name}` };
}

} // namespace elizaos
