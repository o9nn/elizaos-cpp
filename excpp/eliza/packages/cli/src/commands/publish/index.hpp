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
#include "actions/github-publish.hpp"
#include "actions/npm-publish.hpp"
#include "actions/registry-publish.hpp"
#include "types.hpp"
#include "utils/authentication.hpp"
#include "utils/metadata.hpp"
#include "utils/validation.hpp"
#include "utils/version-check.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Import modular actions

// Import utilities

// Import types

// Constants

      // Use standardized directory detection

      // Validate that we're in a valid directory with package.json

      // Check for CLI updates

      // Get the plugin directory name (should be plugin-*)

      // Validate we're in a plugin directory

      // Validate data directory and settings only if we need GitHub publishing

          // Initialize data directory first

          // Use the built-in credentials function

          // Revalidate after saving credentials

      // Read package.json

      // Use standardized directory detection for type determination

        // Fallback for backwards compatibility - check package.json fields

      // Add packageType and platform to package.json for publishing

      // Preserve agentConfig if it exists or create it
        // Ensure pluginType is set based on detection

      // For plugin type, validate naming convention

      // Get GitHub credentials only if we need them (not npm-only mode)

      // Get NPM username for registry compliance

      // Replace ALL placeholders in package.json

        // Template default name replacement
        // Name placeholders (for custom templates)
        // Description placeholder
        // Repository URL placeholder (only for GitHub publishing)
        // Author placeholder (only for GitHub publishing)
        // Bugs URL placeholder (only for GitHub publishing)

      // Apply all placeholder replacements

      // Extract final plugin name for use in publishing

      // Save updated package.json

      // Validate plugin requirements

      // Update registry settings

      // Generate package metadata

      // Check if user is a maintainer

      // Handle dry run mode (create local registry files)

        // Save package to local registry

        // Test registry publishing

      // Track what was actually published for accurate messaging

      // Step 1: Publish to npm (always, unless we add a --skip-npm flag later)

      // Add npm package info to metadata

      // Step 2: Publish to GitHub and registry (unless --npm flag is used for npm-only)

          // Add GitHub repo info to metadata

          // Store PR URL if returned from publishToGitHub (only show if registry not skipped)

      // Handle registry publication messaging

      // Show availability URLs only for platforms where we actually published

// Re-for backward compatibility
* from './actions/npm-publish';
* from './actions/github-publish';
* from './actions/registry-publish';
* from './utils/validation';
* from './utils/metadata';
* from './utils/authentication';
* from './utils/version-check';
* from './types';

} // namespace elizaos
