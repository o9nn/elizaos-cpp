#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".pipelineConfig.hpp"
#include ".types.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Context for ingestion pipeline operations
 */
  /** GitHub client for API requests */
  /** Flag to force fetch data regardless of lastFetched timestamp */

struct CreateIngestionContextOptions {
    std::string repoId;
    Logger logger;
    PipelineConfig config;
    DateRange dateRange;
    bool force;
    std::string githubToken;
};

/**
 * Create a context for ingestion pipeline
 */
IngestionPipelineContext createIngestionContext(auto logger, auto config, auto dateRange, auto force, auto githubToken);

} // namespace elizaos
