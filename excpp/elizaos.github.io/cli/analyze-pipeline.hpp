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

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * GitHub Contribution Analytics Pipeline
 *
 * This script manages the full lifecycle of GitHub contribution analysis:
 * 1. Fetching data from GitHub using GraphQL API
 * 2. Storing raw data in SQLite database
 * 3. Processing raw data through modular processing steps
 */

// Load environment variables from .env file

// Validate required environment variables

// Ingest data from GitHub

      // Create a root logger with appropriate log level

      // Calculate date range using shared helper

      // Create ingestion context with date range

      // Run the ingestion pipeline - returns array of { repository, prs, issues }

// Process and analyze data

      // Create a root logger

      // Create pipeline context with the root logger and force option

      // Run the pipeline directly - no need for the ContributorPipeline class

// Export repository stats

      // Create a root logger

      // If --all is passed, ensure contributionStartDate is set

      // Calculate date range using shared helper

      // Create pipeline context

      // Run the repository summaries pipeline

// Generate summaries

      // Create a root logger

      // Validate summary type

      // If --all is passed, ensure contributionStartDate is set

      // Calculate date range using shared helper

      // If no interval flags are set, enable all intervals

      // Create summarizer context

      // Run the appropriate pipeline based on summary type


} // namespace elizaos
