#include "context.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

IngestionPipelineContext createIngestionContext(auto {
  repoId, auto logger, auto config, auto dateRange, auto force = false, auto githubToken, CreateIngestionContextOptions }) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Create a GitHub client with the provided logger or a child logger
    const auto githubLogger = logger.child("GitHub");
    const auto github = new GitHubClient(githubToken, githubLogger);

    return {
        config,
        logger,
        github,
        repoId,
        dateRange,
        force,
        };

}

} // namespace elizaos
