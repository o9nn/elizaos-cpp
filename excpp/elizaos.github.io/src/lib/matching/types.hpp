#include ".data/tags.hpp"
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

/**
 * Enum for pattern matching method types
 */
enum PatternType {
  STRING = "STRING", // Simple string matching
  REGEX = "REGEX", // Regular expression
  GLOB = "GLOB", // Glob pattern (for file paths)
}

/**
 * Interface for pattern matching options
 */
struct PatternOptions {
    std::optional<boolean; // Whether matching should be case sensitive (default: false)> caseSensitive;
    std::optional<boolean; // Whether to match whole words only (default: false)> wordBoundary;
    std::optional<boolean; // Whether this is a negative pattern (match means exclusion) (default: false)> negative;
};


/**
 * Interface defining a pattern to be matched against content
 */
struct TagPattern {
    string; // The pattern string to match pattern;
    PatternType; // Type of pattern (string, regex, glob) patternType;
    TagPatternType; // Type of content to match against target;
    PatternOptions; // Matching options options;
};


/**
 * Type for content that can be matched
 */
using MatchContent = {
  content: string; // The actual content to match against
  contentType: TagPatternType; // The type of content
};

} // namespace elizaos
