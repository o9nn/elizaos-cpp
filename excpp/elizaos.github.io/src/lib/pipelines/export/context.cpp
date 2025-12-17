#include "context.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

RepositoryStatsPipelineContext createRepositoryStatsPipelineContext(std::optional<std::any> params) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto {;
        repoId,
        dateRange,
        outputDir,
        logger: parentLogger,
        config,
        overwrite = false,
        } = params;

        // Use parent logger if provided, creating a child logger for RepositoryStats
        const auto logger = parentLogger ? parentLogger.child("Export") : std::nullopt;

        return {
            repoId,
            dateRange,
            outputDir,
            logger,
            config,
            overwrite,
            };

}

} // namespace elizaos
