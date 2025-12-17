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



/**
 * Normalizes a plugin input string to a standard format, typically 'plugin-name'.
 * Used primarily for display and generating commands in bunx instructions.
 */

  // Handle scoped formats like "@scope/plugin-name" or "scope/plugin-name"
  // Remove potential scope from "@plugin-name" - less common but possible
      // Assume it's like "@something" without a scope/name separator - maybe log a warning?
      // For now, let's just take the part after '@'

  // Ensure it starts with 'plugin-' and remove duplicates if necessary

/**
 * Finds the actual package name in dependencies based on various input formats.
 */
  // Normalize the input to a base form (e.g., 'abc' from 'plugin-abc')

  // Potential package names to check (prioritize @elizaos/ over @elizaos/)

/**
 * Extracts the actual npm package name from various input formats.
 * This function handles GitHub URLs, package names, and repository names
 * but preserves the exact package name for installation.
 */
  // Handle GitHub URLs and repository names

  // Check for GitHub URL format first
    // For GitHub repos, we typically expect the package name to be scoped
    // e.g., github:elizaos-plugins/plugin-discord -> @elizaos/plugin-discord

  // Check for GitHub shorthand format

  // Return the input as-is for regular package names

} // namespace elizaos
