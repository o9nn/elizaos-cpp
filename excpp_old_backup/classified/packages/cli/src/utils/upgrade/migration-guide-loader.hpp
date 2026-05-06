#include "elizaos/core.hpp"
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



struct MigrationGuide {
    std::string name;
    std::string path;
    std::string content;
    'basic' | 'advanced' | 'testing' | 'completion' category;
    std::vector<std::string> keywords;
};

struct GuideSearchResult {
    MigrationGuide guide;
    double relevanceScore;
    std::vector<std::string> matchedKeywords;
};

class MigrationGuideLoader {
  private guides: MigrationGuide[] = [];
  private guidesDir: string;

  constructor(projectRoot?: string) {
    // Find guides directory with multiple fallback strategies
    this.guidesDir = this.findGuidesDirectory(projectRoot);
    this.loadGuides();
  }

    // Strategy 1: Use provided project root

    // Strategy 2: Find project root from current working directory

    // Strategy 3: Look for guides in the working directory (copied guides)

    // Strategy 4: Look relative to CLI package

    // Strategy 5: Return empty directory - will use embedded guidance

    // If no guides directory found, create embedded guides

    // Load guides from filesystem

    // Create minimal embedded guides with essential migration information

  /**
   * Search for relevant guides based on keywords or content
   */

      // Check keyword matches

      // Check content matches

      // Boost certain guides for common issues

  /**
   * Get a specific guide by name
   */

  /**
   * Get all guides of a specific category
   */

  /**
   * Get guides that are most relevant for common migration issues
   */

  /**
   * Generate a comprehensive migration context for Claude
   */

      // Include first 500 characters as preview

  /**
   * Get full content of all guides for RAG embedding
   */

/**
 * Helper function to create a guide loader instance
 */
MigrationGuideLoader createMigrationGuideLoader(std::optional<std::string> projectRoot);

/**
 * Helper function to get migration context for common issues
 */
std::string getMigrationHelpForIssue(const std::string& issue, std::optional<std::string> projectRoot);

} // namespace elizaos
