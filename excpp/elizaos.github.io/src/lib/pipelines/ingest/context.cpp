#include "context.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

IngestionPipelineContext createIngestionContext(auto logger, auto config, auto dateRange, auto force, auto githubToken) {
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
