#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "pipelineConfig.hpp"
#include "types.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Extended context for contributor pipelines
 */

/**
 * Create a context for contributor pipelines
 */
ContributorPipelineContext createContributorPipelineContext(auto logger, auto config, auto force);

} // namespace elizaos
