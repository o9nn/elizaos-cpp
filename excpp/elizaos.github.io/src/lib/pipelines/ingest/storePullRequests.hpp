#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".types.hpp"
#include "context.hpp"
#include "mutations.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Step to fetch and store pull requests for a repository
 */

    // Record the ingestion start time

      // Fetch pull requests from GitHub

      // Filter out undefined values

      // Collect all users that need to be created/updated
        // PR author

        // Review authors

        // Comment authors

        // Commit authors

      // Ensure all users exist in a single batch operation

      // Process all labels first

      // Batch insert PRs

        // Store PR-Label relationships

      // Batch insert PR files

      // Batch insert commits

      // Batch insert reviews

      // Batch insert PR comments

          // Process reactions on comments

      // Process PR reactions

      // Process PR closing issue references


} // namespace elizaos
