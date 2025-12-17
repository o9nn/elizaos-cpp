#include "context.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

ContributorPipelineContext createContributorPipelineContext(auto {
  repoId, auto logger, auto config, auto force = false, std::optional<std::any> }) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return {
        repoId,
        logger,
        config,
        force,
        };

}

} // namespace elizaos
