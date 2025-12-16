#include ".types.hpp"
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

interface RepositoryStatsPipelineContext extends RepoPipelineContext {
  outputDir: string;
  overwrite: boolean;
}

/**
 * Creates a repository stats pipeline context
 */
): RepositoryStatsPipelineContext {
  const {
    repoId,
    dateRange,
    outputDir,
    logger: parentLogger,
    config,
    overwrite = false,
  } = params;

  // Use parent logger if provided, creating a child logger for RepositoryStats
  const logger = parentLogger ? parentLogger.child("Export") : undefined;

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
