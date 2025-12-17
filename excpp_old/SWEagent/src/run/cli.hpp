#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".agent/agents.hpp"
#include ".agent/models.hpp"
#include ".agent/problem-statement.hpp"
#include ".environment/repo.hpp"
#include ".environment/swe-env.hpp"
#include "run-batch.hpp"
#include "run-single.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * Main command line interface
 */

// Removed unused CLIConfig import

// Run command
      // Parse options into config

      // Handle nested options

        // Load text from file

// Run-batch command
      // Parse options into config

      // Handle instance configuration

      // Handle agent configuration
          // For retry agents or if agent not configured, create a default agent config

// Merge-preds command

// Inspector command

// Quick-stats command

// Remove-unfinished command

// Compare-runs command

// Extract-pred command

// Traj-to-demo command

// Run-replay command

// Shell command

// Parse arguments

// If no arguments provided, show help and exit with error

} // namespace elizaos
