#include ".getSelectedRepositories.hpp"
#include ".types.hpp"
#include "context.hpp"
#include "fetchWalletAddresses.hpp"
#include "pipeline.hpp"
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

{ createIngestionContext };

// Pipeline for generating monthly project summaries
const ingestPipeline = pipe(
  getSelectedRepositories,
  createStep("mapRepos", (repositories) => {
    return repositories.map(({ repoId, owner, name, defaultBranch }) => ({
      repository: { repoId, owner, name, defaultBranch },
    }));
  }),
  mapStep(ingestWeeklyGithubData),
  createStep("Log Project Summaries", (results, context) => {
    for (const intervals of results) {
      const totalPrs = intervals.reduce((acc, interval) => {
        return acc + interval.prs;
      }, 0);
      const totalIssues = intervals.reduce((acc, interval) => {
        return acc + interval.issues;
      }, 0);
      context.logger?.info(
        `Ingested ${totalPrs} total PRs and ${totalIssues} total issues across ${intervals.length} weeks`,
      );
    }
  }),
  createStep("WalletAddress", fetchWalletAddresses),
);

} // namespace elizaos
