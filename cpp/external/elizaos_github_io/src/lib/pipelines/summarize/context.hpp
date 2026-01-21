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
#include "config.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Pipeline context for contributor and project summaries
 */
  /** Output directory for summary files */
  /** AI summary configuration */
  /** Logger instance */
  /** Whether to overwrite existing summaries */
  /** Date range for filtering data */
  /** Which interval types are enabled for summary generation */

struct CreateContributorSummaryContextOptions {
    std::optional<std::string> repoId;
    PipelineConfig config;
    std::optional<Logger> logger;
    std::string outputDir;
    AISummaryConfig aiSummaryConfig;
    std::optional<bool> overwrite;
};

/**
 * Create a context for contributor summary pipelines
 */
SummarizerPipelineContext createSummarizerContext(CreateContributorSummaryContextOptions options);

} // namespace elizaos
