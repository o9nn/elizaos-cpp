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
;
;

/**
 * Context for ingestion pipeline operations
 */
interface IngestionPipelineContext extends RepoPipelineContext {
  /** GitHub client for API requests */
  github: GitHubClient;
  /** Flag to force fetch data regardless of lastFetched timestamp */
  force: boolean;
}

struct CreateIngestionContextOptions {
    string | undefined repoId;
    Logger logger;
    PipelineConfig config;
    DateRange dateRange;
    bool force;
    std::string githubToken;
};


/**
 * Create a context for ingestion pipeline
 */
: CreateIngestionContextOptions): IngestionPipelineContext {
  // Create a GitHub client with the provided logger or a child logger
  const githubLogger = logger?.child("GitHub");
  const github = new GitHubClient(githubToken, githubLogger);

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
