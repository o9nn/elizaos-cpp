#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".types.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Default watcher configuration
 */

/**
 * Find TypeScript/JavaScript files in a directory
 */
std::vector<std::string> findTsFiles(const std::string& dir, const std::string& watchDir);

/**
 * Sets up file watching for the given directory
 *
 * Watches for changes to TypeScript and JavaScript files, with debouncing to prevent rapid rebuilds.
 */
    // Get the absolute path of the directory

    // Use a simpler approach - watch the src directory directly

    // Merge config with defaults

    // Create a more direct and simple watcher pattern

    // Manually find TypeScript files to verify we should be watching them

    // On ready handler

        // Try an alternative approach with explicit file patterns

    // Set up file change handler
      // Only react to specific file types

      // Debounce the onChange handler to avoid multiple rapid rebuilds

    // Add an error handler

    // Ensure proper cleanup on process exit

/**
 * Create a debounced file change handler
 */


} // namespace elizaos
