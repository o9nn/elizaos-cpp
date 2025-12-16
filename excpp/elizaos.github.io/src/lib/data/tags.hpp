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
 * TagPatternType defines the different types of pattern matching
 */
enum TagPatternType {
  FILE_PATH = "FILE_PATH", // Match against file paths
  COMMIT_MESSAGE = "COMMIT_MESSAGE", // Match against commit messages
  CODE_CONTENT = "CODE_CONTENT", // Match actual code content
  PR_TITLE = "PR_TITLE", // Match PR titles
  PR_DESCRIPTION = "PR_DESCRIPTION", // Match PR descriptions
  PR_CLOSES_ISSUE = "PR_CLOSES_ISSUE", // Match PRs that close issues
  ISSUE_TITLE = "ISSUE_TITLE", // Match issue titles
  ISSUE_BODY = "ISSUE_BODY", // Match issue content
  COMMENT = "COMMENT", // Match comment content
  LABEL = "LABEL", // Match labels
  REACTION = "REACTION", // Match reactions (content, user, and count)
}

/**
 * TagCategory defines the main categories for tag classification
 */
enum TagCategory {
  AREA = "AREA", // Domain/feature area: frontend, backend, etc.
  ROLE = "ROLE", // Role-based: developer, designer, reviewer, etc.
  TECH = "TECH", // Technology: React, Python, AWS, etc.
}

// Zod schemas for validation

/**
 * Schema for tag pattern scoring mechanics
 */

/**
 * Schema for a tag pattern that defines how to match activities to tags
 */

/**
 * Schema for a tag rule that groups patterns for a specific skill/area
 */

// Type exports from zod schemas
using TagScoring = z.infer<typeof TagScoringSchema>;
using TagPattern = z.infer<typeof TagPatternSchema>;
using TagRule = z.infer<typeof TagRuleSchema>;

} // namespace elizaos
