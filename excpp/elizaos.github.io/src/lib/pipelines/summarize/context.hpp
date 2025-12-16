#include "config.hpp"
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

/**
 * Pipeline context for contributor and project summaries
 */
interface SummarizerPipelineContext extends RepoPipelineContext {
  /** Output directory for summary files */
  outputDir: string;
  /** AI summary configuration */
  aiSummaryConfig: AISummaryConfig;
  /** Logger instance */
  logger?: Logger;
  /** Whether to overwrite existing summaries */
  overwrite?: boolean;
  /** Date range for filtering data */
  dateRange: { startDate: string; endDate?: string };
  /** Which interval types are enabled for summary generation */
  enabledIntervals: Record<IntervalType, boolean>;
}

struct CreateContributorSummaryContextOptions {
    std::optional<std::string> repoId;
    PipelineConfig config;
    std::optional<Logger> logger;
    std::string outputDir;
    AISummaryConfig aiSummaryConfig;
    std::optional<bool> overwrite;
    { startDate: string; endDate?: string } dateRange;
    std::unordered_map<IntervalType, bool> enabledIntervals;
};


/**
 * Create a context for contributor summary pipelines
 */
;
}

} // namespace elizaos
