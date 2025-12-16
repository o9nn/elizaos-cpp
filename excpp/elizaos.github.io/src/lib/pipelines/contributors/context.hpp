#include ".pipelineConfig.hpp"
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
;
;

/**
 * Extended context for contributor pipelines
 */
interface ContributorPipelineContext extends RepoPipelineContext {
  force?: boolean; // Whether to force recalculation of scores
}

/**
 * Create a context for contributor pipelines
 */
: {
  repoId?: string;
  logger?: Logger;
  config: PipelineConfig;
  force?: boolean;
}): ContributorPipelineContext {
  return {
    repoId,
    logger,
    config,
    force,
  };
}

} // namespace elizaos
