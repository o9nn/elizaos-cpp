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
#include "general.hpp"
#include "validators.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * CLI tool for validating files against SWE-agent rules
 */

/**
 * Validate command
 */
        // If no files specified, validate current directory

        // Expand wildcards and resolve paths

        // Validate all files

        // Filter results based on options

        // Format and display results

        // Check if any errors

/**
 * List rules command
 */

/**
 * Export rules command
 */

      // Create output directory if it doesn't exist

      // Export all rules

/**
 * Info command
 */

// Helper functions

// Parse command line arguments

// Show help if no command provided

} // namespace elizaos
